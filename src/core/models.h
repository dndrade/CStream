#ifndef MODELS_H
#define MODELS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>



extern const char *ERROR_CODE_STRINGS[];

enum errorCode {
    ERR_OK = 200,
    ERR_BAD_REQUEST = 400,
    ERR_NOT_FOUND = 404,
    ERR_INTERNAL = 500,
    ERR_NOT_IMPLEMENTED = 501,
    ERR_UNAVAILABLE = 503
};

typedef enum {
    MDB_STATUS_FOUND = 0,
    MDB_STATUS_NOT_FOUND,
    MDB_STATUS_EMPTY
} mdb_request_state;

typedef enum {
    ERROR_SUCCESS = 0,
    ERROR_WRITE_FAILED,
    ERROR_INVALID_INPUT
} error_state;

typedef enum {
    UTILS_SUCCESS = 0,
    UTILS_INVALID_INPUT,
    UTILS_ALLOC_FAILED,
    UTILS_IO_ERROR
} utils_state;

typedef enum {
    SERVER_SUCCESS = 0,
    SERVER_INIT_FAILED,
    SERVER_SOCKET_ERROR,
    SERVER_CONFIG_ERROR,
    SERVER_CLIENT_ERROR
} server_state;

typedef enum {
    MDB_HANDLER_SUCCESS = 0,
    MDB_HANDLER_QUERY_ERROR,
    MDB_HANDLER_READ_ERROR,
    MDB_HANDLER_WRITE_ERROR
} mdbHandler_state;

typedef enum {
    HTTP_SUCCESS = 0,
    HTTP_FILE_ERROR,
    HTTP_WRITE_ERROR,
    HTTP_FORMAT_ERROR
} http_state;

typedef struct {
    char *data;
    size_t size;
    size_t used;
} FormatBuffer;

#define HTTP_TEMPLATE_200 \
    "HTTP/1.0 200 OK\r\n" \
    "Content-Type: %s\r\n" \
    "Connection: close\r\n" \
    "\r\n"

#define HTTP_TEMPLATE_404 \
    "HTTP/1.0 404 Not Found\r\n" \
    "Content-Type: text/html\r\n" \
    "Connection: close\r\n" \
    "\r\n"

#define HTML_INDEX_TEMPLATE \
    "<html>\n" \
    "<body bgcolor=\"black\" text=\"white\" link=\"white\" vlink=\"white\">\n" \
    "<center>\n" \
    "<br><br><br>\n" \
    "<h1>MDB Lookup</h1>\n" \
    "<br><br><br>\n" \
    "<form action=\"/mdb-lookup\" method=\"GET\">\n" \
    "<input type=\"text\" name=\"key\">\n" \
    "<input type=\"submit\" value=\"Lookup\">\n" \
    "</form>\n" \
    "</center>\n" \
    "</body>\n" \
    "</html>\n"

#define HTML_RESPONSE_TEMPLATE \
    "<html><body bgcolor=\"black\" text=\"white\">\n" \
    "<center>\n" \
    "<br><br><br>\n" \
    "<h1>MDB Lookup</h1>\n" \
    "<h3>Records</h3>\n" \
    "<br><br><br>\n" \
    "<table width=\"50%%\" border=1><tr><td>\n" \
    "<pre>\n%s</pre>\n" \
    "</td></tr></table>\n" \
    "<hr>\n" \
    "<a href=\"/\">New lookup</a>\n" \
    "</center>\n" \
    "</body></html>\n"


#define HTML_ERROR_TEMPLATE \
"<html><body><h1>%d %s</h1></body></html>"


typedef struct {
    size_t size;
    const char* name;
} BufferConfig;

static const BufferConfig BUFFER_SIZES[] = {
    {256, "SMALL"},
    {4096, "MEDIUM"},
    {8192, "LARGE"}
};

#define SMALL_BUFF_SIZE BUFFER_SIZES[0].size
#define MEDIUM_BUFF_SIZE BUFFER_SIZES[1].size
#define LARGE_BUFF_SIZE BUFFER_SIZES[2].size


typedef struct {
    char *web_root;
    char *mdb_host;
    unsigned short port;
    unsigned short mdbport;
} HTTPConf;

typedef struct {
    char *ip;
    int clntsock;
    struct sockaddr_in clntaddr;
} Client;

typedef struct {
    int servsock;
    int mdbsock;
    HTTPConf staticConfig;
    socklen_t clntlen;
    struct sockaddr_in servaddr;
    Client *activeClient;
} WebServer;

#endif