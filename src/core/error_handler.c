#include "error_handler.h"
#include "logger.h"
#include <string.h>
#include <unistd.h>


error_state fatal_error_handler(WebServer *server, const char *msg) {
    if (!msg) return ERROR_INVALID_INPUT;
    
    log_error(NULL, msg, ERR_INTERNAL);
    return ERROR_WRITE_FAILED;
}

error_state error_handler(WebServer *server, enum errorCode code, const char *msg) {
    if (!server || !msg) return ERROR_INVALID_INPUT;
    
    const char *ip = server->activeClient ? (const char*)server->activeClient->ip : NULL;
    log_error(ip, msg, code);
    return ERROR_WRITE_FAILED;
}

error_state send_error_response(WebServer *server, enum errorCode code, const char *msg) {
    if (!server || !msg) return ERROR_INVALID_INPUT;
    
    char buffer[LARGE_BUFF_SIZE];
    format_result_t result = format_error_page(code, 
                                             ERROR_CODE_STRINGS[code], 
                                             buffer, 
                                             sizeof(buffer));
    
    if (result != FORMAT_SUCCESS) {
        return ERROR_WRITE_FAILED;
    }
    
    if (server->activeClient) {
        ssize_t written = write(server->activeClient->clntsock, 
                              buffer, 
                              strlen(buffer));
        return (written > 0) ? ERROR_SUCCESS : ERROR_WRITE_FAILED;
    }
    
    return ERROR_SUCCESS;
}
