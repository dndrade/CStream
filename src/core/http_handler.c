#include "http_handler.h"
#include "utils.h"
#include "error_handler.h"
#include "format.h"
#include "logger.h"
#include <limits.h>
#include <string.h>

#include "http_handler.h"

static inline http_state send_file_content(WebServer* server, FILE* file)
{
    char buffer[LARGE_BUFF_SIZE];
    size_t bytes;
    ssize_t bytes_written;

    while ((bytes = fread(buffer, 1, LARGE_BUFF_SIZE, file)) > 0)
    {
        utils_state result = write_socket(
            server->activeClient->clntsock,
            buffer,
            bytes,
            &bytes_written
        );

        if (result != UTILS_SUCCESS)
        {
            fclose(file);
            error_handler(server, ERR_INTERNAL, "Socket write failed");
            return HTTP_WRITE_ERROR;
        }
    }

    fclose(file);
    return HTTP_SUCCESS;
}

static inline http_state send_response(WebServer* server,
                                        const char* content_type,
                                        const char* content,
                                        size_t content_len) {
    char headers[MEDIUM_BUFF_SIZE];
    ssize_t bytes_written;

    // Format and send headers first
    format_state fmt_result = format_http_headers(content_type, headers, MEDIUM_BUFF_SIZE);
    if (fmt_result != FORMAT_SUCCESS) return HTTP_FORMAT_ERROR;

    utils_state write_result = write_socket(
        server->activeClient->clntsock,
        headers,
        strlen(headers),
        &bytes_written
    );
    if (write_result != UTILS_SUCCESS) return HTTP_WRITE_ERROR;

    write_result = write_socket(
        server->activeClient->clntsock,
        content,
        content_len,
        &bytes_written
    );
    return (write_result == UTILS_SUCCESS) ? HTTP_SUCCESS : HTTP_WRITE_ERROR;
}


static inline http_state send_index_page(WebServer* server) {
    char response[LARGE_BUFF_SIZE];


    snprintf(response, LARGE_BUFF_SIZE,
             "HTTP/1.0 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s", HTML_INDEX_TEMPLATE);

    ssize_t bytes_written;
    return write_socket(
        server->activeClient->clntsock,
        response,
        strlen(response),
        &bytes_written
    ) == UTILS_SUCCESS ? HTTP_SUCCESS : HTTP_WRITE_ERROR;
}



static inline http_state send_static_file(WebServer* server, const char* uri)
{
    char* fullpath;
    if (format_path(server->staticConfig.web_root, uri, &fullpath) != UTILS_SUCCESS)
        return HTTP_FORMAT_ERROR;

    FILE* file;
    utils_state file_result = ap_open(fullpath, "rb", &file);
    free(fullpath);

    if (file_result != UTILS_SUCCESS)
    {
        error_handler(server, ERR_NOT_FOUND, "File not found");
        return HTTP_FILE_ERROR;
    }

    char headers[MEDIUM_BUFF_SIZE];
    if (format_http_headers(get_content_type(uri), headers, MEDIUM_BUFF_SIZE)
        != FORMAT_SUCCESS)
    {
        fclose(file);
        return HTTP_FORMAT_ERROR;
    }

    ssize_t bytes_written;
    if (write_socket(server->activeClient->clntsock, headers, strlen(headers),
        &bytes_written) != UTILS_SUCCESS)
    {
        fclose(file);
        return HTTP_WRITE_ERROR;
    }

    return send_file_content(server, file);
}

const char* get_content_type(const char* path)
{
    const char* ext = strchr(path, '.');
    if (!ext) return "text/plain";
    ext++;

    if (strcmp(ext, "html") == 0) return "text/html";
    if (strcmp(ext, "jpg") == 0)  return "image/jpeg";

    return "text/plain";
}


http_state serve_static(WebServer* server, const char* uri) {
    fprintf(stderr, "Serving request for URI: %s\n", uri);

    if (strcmp(uri, "/favicon.ico") == 0) {
        ssize_t bytes_written;
        const char* response =
            "HTTP/1.0 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n";

        write_socket(server->activeClient->clntsock, response, strlen(response), &bytes_written);
        return HTTP_FILE_ERROR;
    }

    if (strcmp(uri, "/") == 0) {
        fprintf(stderr, "Serving index page\n");
        return send_index_page(server);
    } else {
        fprintf(stderr, "Serving static file\n");
        return send_static_file(server, uri);
    }
}

