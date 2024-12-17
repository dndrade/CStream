#include <stdlib.h>
#include "server.h"
#include "logger.h"


int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr,
                "Usage: %s <server_port> <web_root> <mdb-lookup-host> <mdb-lookup-port>\n",
                argv[0]
                );
        return EXIT_FAILURE;
    }

    WebServer *server;
    const char *ports[] = {argv[1], argv[4]};

    server_state result = init_server(argv[2], ports, argv[3], &server);
    if (result != SERVER_SUCCESS) {
        log_error(NULL, "Server initialization failed", result);
        return EXIT_FAILURE;
    }

    result = run_server(server);
    if (result != SERVER_SUCCESS) {
        log_error(NULL, "Server execution failed", result);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
