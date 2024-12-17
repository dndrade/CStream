#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H


#include "models.h"


http_state serve_static(WebServer* server, const char* uri);
const char* get_content_type(const char* path);


#endif
