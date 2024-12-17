#ifndef MDB_HANDLER_H
#define MDB_HANDLER_H


#include "models.h"
#include "utils.h"
#include "error_handler.h"
#include "format.h"
#include "logger.h"


http_state process_lookup(WebServer *server, const char *query);

#endif