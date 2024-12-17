#ifndef FORMAT_H
#define FORMAT_H


#include "models.h"


typedef enum {
    RESPONSE_HTML,
    RESPONSE_ERROR,
    RESPONSE_INDEX,
    RESPONSE_HEADERS
} ResponseType;

typedef enum {
    FORMAT_SUCCESS = 0,
    FORMAT_BUFFER_FULL,
    FORMAT_INVALID_INPUT
} format_state;

typedef struct {
    ResponseType type;
    const char* content;
    size_t buffer_size;
} FormatRequest;


format_state format_response(const FormatRequest* req, char* output);
format_state format_msg_str(const char* template, const char* content,
                             char* output, size_t size);
format_state format_msg_int(const char* template, int content,
                             char* output, size_t size);
char* format_client_connect(const char* ip);
char* format_client_disconnect(const char* ip);


#define FORMAT_CHECK(expr) do { \
    format_state result = (expr); \
    if (result != FORMAT_SUCCESS) return result; \
} while(0)


#define FORMAT_SERVER(cmd, arg, out, size) \
    format_msg_str(SERVER_##cmd##_TEMPLATE, arg, out, size)


#define format_server_start(port, out, size) \
    format_msg_int(LOG_SERVER_START, port, out, size)
#define format_cleanup(resource, out, size) \
    format_msg_str(LOG_CLEANUP, resource, out, size)


#define FORMAT_ERROR(type, arg, out, size) \
    format_msg_str(ERROR_##type##_TEMPLATE, arg, out, size)


#define format_alloc_error(thing, out, size) \
    format_msg_str(MSG_ALLOC_ERROR, thing, out, size)
#define format_close_error(server, out, size) \
    format_msg_str(MSG_CLOSE_ERROR, server, out, size)
#define format_connect_error(server, out, size) \
    format_msg_str(MSG_CONNECT_ERROR, server, out, size)
#define format_path_error(issue, out, size) \
    format_msg_str(MSG_PATH_ERROR, issue, out, size)
#define format_socket_error(operation, out, size) \
    format_msg_str(MSG_SOCKET_ERROR, operation, out, size)


#define format_mdb_query(query, out, size) \
    format_msg_str(MSG_MDB_QUERY, query, out, size)
#define format_mdb_not_found(query, out, size) \
    format_msg_str(MSG_MDB_NOT_FOUND, query, out, size)
#define format_mdb_error(error, out, size) \
    format_msg_str(MSG_MDB_ERROR, error, out, size)


#define format_response_ok(type, out, size) \
    format_msg_str(HTTP_TEMPLATE_200, type, out, size)
#define format_response_not_found(out, size) \
    format_msg_str(HTTP_TEMPLATE_404, "", out, size)
#define format_response_bad_request(out, size) \
    format_msg_str(HTTP_TEMPLATE_400, "", out, size)
#define format_response_server_error(out, size) \
    format_msg_str(HTTP_TEMPLATE_500, "", out, size)
#define format_response_not_implemented(out, size) \
    format_msg_str(HTTP_TEMPLATE_501, "", out, size)



format_state format_response(const FormatRequest* req, char* output);
format_state format_html_response(const char* message, mdb_request_state status,
                                   char* output, size_t size);
format_state format_error_page(int code, const char* status,
                                char* output, size_t size);
format_state format_index_page(char* output, size_t size);
format_state format_http_headers(const char* type,
                                  char* output, size_t size);


#endif
