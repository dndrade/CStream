#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H


#include "models.h"
#include "format.h"
/*
 * REFERENCES 
 * https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
 * https://github.com/omnigroup/Apache/blob/master/httpd/modules/loggers/mod_log_config.c
 * https://github.com/nginx/nginx/blob/master/src/core/ngx_log.h
 */


#define VALIDATE_METHOD(method, server) do { \
    if (strcmp(method, "GET") != 0) { \
        return error_handler(server, ERR_NOT_IMPLEMENTED, MSG_METHOD_ERROR); \
    } \
} while(0)

#define VALIDATE_URI(uri, server) do { \
    if (uri[0] != '/') { \
        return error_handler(server, ERR_BAD_REQUEST, MSG_URI_ERROR); \
    } \
} while(0)


#define CHECK_ALLOC(ptr, server, thing) do { \
    if (!(ptr)) { \
        return fatal_error_handler(server, format_alloc_error(thing)); \
    } \
} while(0)

#define CHECK_SOCKET(result, server, operation) do { \
    if ((result) < 0) { \
        return fatal_error_handler(server, format_socket_error(operation)); \
    } \
} while(0)


error_state fatal_error_handler(WebServer *server, const char *msg);
error_state error_handler(WebServer *server, enum errorCode code, const char *msg);
error_state send_error_response(WebServer *server, enum errorCode code, const char *msg);


#endif
