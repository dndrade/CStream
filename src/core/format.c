#include "format.h"
#include "msg.h"
#include <stdio.h>
#include <string.h>


format_state format_response(const FormatRequest* req, char* output) {
    if (!req || !output) return FORMAT_INVALID_INPUT;

    const char* template = NULL;
    switch(req->type) {
        case RESPONSE_HTML:
            template = HTML_RESPONSE_TEMPLATE;
            break;
        case RESPONSE_ERROR:
            template = HTML_ERROR_TEMPLATE;
            break;
        case RESPONSE_INDEX:
            template = HTML_INDEX_TEMPLATE;
            break;
        case RESPONSE_HEADERS:
            template = HTTP_TEMPLATE_200;
            break;
        default:
            return FORMAT_INVALID_INPUT;
    }

    return format_msg_str(template, req->content, output, req->buffer_size);
}

format_state format_msg_str(const char *template, const char *content,
                             char *output, size_t size) {
    if (!template || !output) {
        return FORMAT_INVALID_INPUT;
    }

    int written;
    if (content) {
        written = snprintf(output, size, template, content);
    } else {
        written = snprintf(output, size, "%s", template);
    }

    return (written >= 0 && written < size) ?
           FORMAT_SUCCESS : FORMAT_BUFFER_FULL;
}

format_state format_msg_int(const char* template, int content,
                             char* output, size_t size) {
    if (!template || !output) {
        return FORMAT_INVALID_INPUT;
    }

    int written = snprintf(output, size, template, content);
    return (written >= 0 && written < size) ?
           FORMAT_SUCCESS : FORMAT_BUFFER_FULL;
}


static char msg_buffer[256];

char* format_client_connect(const char* ip) {
    snprintf(msg_buffer, SMALL_BUFF_SIZE, LOG_CLIENT_CONNECT, ip);
    return msg_buffer;
}

char* format_client_disconnect(const char* ip) {
    snprintf(msg_buffer, SMALL_BUFF_SIZE, LOG_CLIENT_DISCONNECT, ip);
    return msg_buffer;
}


format_state format_html_response(const char* message, mdb_request_state status,
                                   char* output, size_t size) {
    const char* display_message;
    switch(status) {
        case MDB_STATUS_FOUND:
            display_message = message;
            break;
        case MDB_STATUS_NOT_FOUND:
            display_message = "No matching records found";
            break;
        default:
            display_message = "Empty response from database";
    }

    char html_content[LARGE_BUFF_SIZE];
    snprintf(html_content, LARGE_BUFF_SIZE, HTML_RESPONSE_TEMPLATE, display_message);

    snprintf(output, size,
             "HTTP/1.0 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Connection: close\r\n"
             "\r\n%s",
             html_content);

    return FORMAT_SUCCESS;
}

format_state format_error_page(int code, const char* status,
                                char* output, size_t size) {
    char error_content[SMALL_BUFF_SIZE];
    snprintf(error_content, sizeof(error_content), "%d %s", code, status);

    FormatRequest req = {
        .type = RESPONSE_ERROR,
        .content = error_content,
        .buffer_size = size
    };

    return format_response(&req, output);
}

format_state format_index_page(char* output, size_t size) {
    FormatRequest req = {
        .type = RESPONSE_INDEX,
        .content = "",
        .buffer_size = size
    };
    return format_response(&req, output);
}

format_state format_http_headers(const char* type,
                                  char* output, size_t size) {
    FormatRequest req = {
        .type = RESPONSE_HEADERS,
        .content = type,
        .buffer_size = size
    };

    return format_response(&req, output);
}
