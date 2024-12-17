#include <string.h>
#include "server.h"
#include <signal.h>
#include <arpa/inet.h>
#include <limits.h>
#include "utils.h"
/*
 * In this part, you are writing a web server, named http-server, that can serve
 * static HTML and image files. The http-server takes the following parameters:
 * ./http-server <server_port> <web_root> <mdb-lookup-host> <mdb-lookup-port>
 *
 */


// https://man7.org/linux/man-pages/man7/signal-safety.7.html
// https://man7.org/linux/man-pages/man2/sigaction.2.html


static int active_server = 1;

static void sigint_handler(int signal) { active_server = 0; }
static void sigterm_handler(int signal) { active_server = 0; }

static server_state setup_signal_handler() {
    struct sigaction sa_int = { .sa_handler = sigint_handler, .sa_flags = 0 };
    struct sigaction sa_term = { .sa_handler = sigterm_handler, .sa_flags = 0 };

    sigemptyset(&sa_int.sa_mask);
    sigemptyset(&sa_term.sa_mask);

    if (sigaction(SIGINT, &sa_int, NULL) < 0 ||
        sigaction(SIGTERM, &sa_term, NULL) < 0) {
        return SERVER_INIT_FAILED;
    }

    signal(SIGPIPE, SIG_IGN);
    return SERVER_SUCCESS;
}

static server_state config_server_port(WebServer *server, const char *ports[]) {
    int port1, port2;
    utils_state result;

    port1 = atoi(ports[0]);
    port2 = atoi(ports[1]);

    result = is_valid_port_number(port1);
    if (result != UTILS_SUCCESS) {
        error_handler(server, ERR_BAD_REQUEST, "Invalid server port");
        return SERVER_CONFIG_ERROR;
    }

    result = is_valid_port_number(port2);
    if (result != UTILS_SUCCESS) {
        error_handler(server, ERR_BAD_REQUEST, "Invalid MDB port");
        return SERVER_CONFIG_ERROR;
    }

    server->staticConfig.port = port1;
    server->staticConfig.mdbport = port2;
    return SERVER_SUCCESS;
}

static server_state init_server_socket(WebServer *server) {
    server->servsock = socket(AF_INET, SOCK_STREAM, 0);
    if (server->servsock < 0) {
        error_handler(server, ERR_INTERNAL, "Socket creation failed");
        return SERVER_SOCKET_ERROR;
    }

    memset(&server->servaddr, 0, sizeof(server->servaddr));
    server->servaddr.sin_family = AF_INET;
    server->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->servaddr.sin_port = htons(server->staticConfig.port);

    if (bind(server->servsock, (struct sockaddr*)&server->servaddr,
            sizeof(server->servaddr)) < 0) {
        error_handler(server, ERR_INTERNAL, "Bind failed");
        return SERVER_SOCKET_ERROR;
    }

    if (listen(server->servsock, 5) < 0) {
        error_handler(server, ERR_INTERNAL, "Listen failed");
        return SERVER_SOCKET_ERROR;
    }

    return SERVER_SUCCESS;
}

static server_state init_mdb_server(WebServer *server) {
    server->mdbsock = socket(AF_INET, SOCK_STREAM, 0);
    if (server->mdbsock < 0) {
        error_handler(server, ERR_UNAVAILABLE, "MDB socket creation failed");
        return SERVER_SOCKET_ERROR;
    }

    struct sockaddr_in mdbaddr;
    memset(&mdbaddr, 0, sizeof(mdbaddr));
    mdbaddr.sin_family = AF_INET;

    in_addr_t in_addr = inet_addr(server->staticConfig.mdb_host);
    if (in_addr == INADDR_NONE) {
        error_handler(server, ERR_UNAVAILABLE, "Invalid MDB host");
        return SERVER_CONFIG_ERROR;
    }

    mdbaddr.sin_addr.s_addr = in_addr;
    mdbaddr.sin_port = htons(server->staticConfig.mdbport);

    if (connect(server->mdbsock, (struct sockaddr*)&mdbaddr, sizeof(mdbaddr)) < 0) {
        error_handler(server, ERR_UNAVAILABLE, "MDB connection failed");
        return SERVER_SOCKET_ERROR;
    }

    log_server("Connected to MDB server");
    return SERVER_SUCCESS;
}

server_state init_server(char *web_root, const char *ports[],
                          char *mdb_host, WebServer **server) {
    *server = NULL;
    utils_state utils_result;

    utils_result = ap_malloc(sizeof(WebServer), (void**)server);
    if (utils_result != UTILS_SUCCESS) {
        return SERVER_INIT_FAILED;
    }

    if (config_server_port(*server, ports) != SERVER_SUCCESS) {
        cleanup_server(*server);
        return SERVER_CONFIG_ERROR;
    }

    utils_result = ap_strlloc(web_root, &(*server)->staticConfig.web_root);
    if (utils_result != UTILS_SUCCESS) {
        cleanup_server(*server);
        return SERVER_INIT_FAILED;
    }

    utils_result = ap_strlloc(mdb_host, &(*server)->staticConfig.mdb_host);
    if (utils_result != UTILS_SUCCESS) {
        cleanup_server(*server);
        return SERVER_INIT_FAILED;
    }

    if (init_server_socket(*server) != SERVER_SUCCESS ||
        init_mdb_server(*server) != SERVER_SUCCESS) {
        cleanup_server(*server);
        return SERVER_INIT_FAILED;
    }

    char msg_buffer[SMALL_BUFF_SIZE];
    format_msg_int(LOG_SERVER_START, (*server)->staticConfig.port,
                  msg_buffer, SMALL_BUFF_SIZE);
    log_server(msg_buffer);

    return SERVER_SUCCESS;
}


static server_state accept_client(WebServer *server, Client **client) {
    struct sockaddr_in clntaddr;
    socklen_t clntlen = sizeof(clntaddr);
    *client = NULL;

    int clntsock = accept(server->servsock, (struct sockaddr*)&clntaddr, &clntlen);
    if (clntsock < 0) {
        error_handler(server, ERR_INTERNAL, "Accept failed");
        return SERVER_CLIENT_ERROR;
    }

    void* client_ptr;
    utils_state result = ap_malloc(sizeof(Client), &client_ptr);
    if (result != UTILS_SUCCESS) {
        close_socket(clntsock);
        return SERVER_CLIENT_ERROR;
    }
    *client = (Client*)client_ptr;

    void* ip_ptr;
    result = ap_malloc(INET_ADDRSTRLEN, &ip_ptr);
    if (result != UTILS_SUCCESS) {
        free(*client);
        close_socket(clntsock);
        return SERVER_CLIENT_ERROR;
    }
    (*client)->ip = (char*)ip_ptr;

    (*client)->clntsock = clntsock;
    inet_ntop(AF_INET, &clntaddr.sin_addr, (*client)->ip, INET_ADDRSTRLEN);

    log_server(format_client_connect((const char*)(*client)->ip));

    return SERVER_SUCCESS;
}


static void free_client(WebServer *server, Client *client)
{
    if (!client) return;

    if (client->clntsock >= 0) {
        close_socket(client->clntsock);
    }

    log_server(format_client_disconnect((const char*)client->ip));

    free(client);
}

server_state run_server(WebServer *server)
{
    if (!server) return SERVER_INIT_FAILED;

    setup_signal_handler();
    char request[MEDIUM_BUFF_SIZE];
    char method[16], uri[PATH_MAX];
    ssize_t bytes_read;
    int is_mdb_lookup;

    while(active_server) {
        server_state result = accept_client(server, &server->activeClient);
        if (result != SERVER_SUCCESS) continue;

        utils_state read_result = read_socket(
            server->activeClient->clntsock,
            request,
            MEDIUM_BUFF_SIZE,
            &bytes_read
        );

        // Get first line of request
        char *end = strchr(request, '\n');
        if (end) *end = '\0';

        if (read_result != UTILS_SUCCESS ||
            sscanf(request, "%15s %s", method, uri) != 2 ||
            starts_with(uri, "/mdb-lookup", &is_mdb_lookup) != UTILS_SUCCESS)
        {
            free_client(server, server->activeClient);
            server->activeClient = NULL;
            continue;
            }

        if (is_mdb_lookup) {
            if (process_lookup(server, request) != HTTP_SUCCESS)
          {
                free_client(server, server->activeClient);
                server->activeClient = NULL;
                continue;
            }
        } else
        {
            if (serve_static(server, uri) != HTTP_SUCCESS)
          {
                free_client(server, server->activeClient);
                server->activeClient = NULL;
                continue;
            }
        }

        free_client(server, server->activeClient);
        server->activeClient = NULL;
    }

    return cleanup_server(server);
}


server_state cleanup_server(WebServer *server)
{
    if (!server) return SERVER_SUCCESS;

    log_server(LOG_SERVER_SHUTDOWN);

    if (server->servsock >= 0) {
        close_socket(server->servsock);
    }

    if (server->mdbsock >= 0) {
        close_socket(server->mdbsock);
    }

    free(server->staticConfig.web_root);
    free(server->staticConfig.mdb_host);

    free_client(server, server->activeClient);
    free(server);

    return SERVER_SUCCESS;
}