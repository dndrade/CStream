#ifndef LOGGER_H
#define LOGGER_H

#include "models.h"
#include "msg.h"
#include <stdio.h>

// basic logging functionality

// https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
// https://github.com/omnigroup/Apache/blob/master/httpd/modules/loggers/mod_log_config.c

typedef enum {
    LOG_SUCCESS = 0,
    LOG_WRITE_ERROR,
    LOG_INVALID_INPUT
} logger_state;


logger_state log_error(const char *ip, const char *msg, int code);
logger_state log_request(const char *ip, const char *method, const char *uri,
                        const char *version, int code);
logger_state log_server(const char *msg);
logger_state log_mdb_query(const char *query, const char *result);



#endif
