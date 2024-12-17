#include "mdb_handler.h"
#include "mdb_handler.h"
#include <string.h>



//static mdb_request_state validate_mdb_response(const char *response, size_t bytes)
//{
//  if (bytes == 0)
//  {
//        char msg_buffer[SMALL_BUFF_SIZE];
//        format_mdb_not_found("empty database", msg_buffer, SMALL_BUFF_SIZE);
//        log_server(msg_buffer);
//        return MDB_STATUS_EMPTY;
//   }
//
//   if (strstr(response, "NOT FOUND"))
//   {
//        char msg_buffer[SMALL_BUFF_SIZE];
//        format_mdb_not_found(response, msg_buffer, SMALL_BUFF_SIZE);
//        log_server(msg_buffer);
//        return  MDB_STATUS_NOT_FOUND;
//    }
//    return MDB_STATUS_FOUND;
//}

mdbHandler_state send_query_to_mdb(WebServer *server, const char *query) {
    fprintf(stderr, "Sending query to MDB: '%s'\n", query);

    ssize_t bytes_written;
    utils_state result = write_socket(server->mdbsock, query, strlen(query), &bytes_written);

    if (result != UTILS_SUCCESS) {
        error_handler(server, ERR_UNAVAILABLE, "MDB write failed");
        return MDB_HANDLER_WRITE_ERROR;
    }

    return MDB_HANDLER_SUCCESS;
}

mdbHandler_state get_response_from_mdb(WebServer *server, char *results, size_t size) {
    fprintf(stderr, "Reading MDB response...\n");

    memset(results, 0, size);
    size_t total_read = 0;
    ssize_t current_read;

    fprintf(stderr, "Starting read loop\n");
    while (total_read < size - 1) {
        utils_state result = read_socket(server->mdbsock,
                                          results + total_read,
                                          size - total_read - 1,
                                          &current_read);

        fprintf(stderr, "Read result: %d, bytes: %zd\n", result, current_read);

        if (result != UTILS_SUCCESS) {
            return MDB_HANDLER_READ_ERROR;
        }

        if (current_read == 0) break;

        total_read += current_read;
        results[total_read] = '\0';

        fprintf(stderr, "Current buffer: %s\n", results);

        if (total_read >= 2 && results[total_read-1] == '\n' && results[total_read-2] == '\n') {
            break;
        }
    }

    return MDB_HANDLER_SUCCESS;
}


//mdbHandler_state process_lookup(WebServer *server, const char *query) {
//    if (!query || strlen(query) == 0) {
//        error_handler(server, ERR_BAD_REQUEST, "Empty query");
//        return MDB_HANDLER_QUERY_ERROR;
//    }
//
//    if (send_query_to_mdb(server, query) != MDB_HANDLER_SUCCESS) {
//        return MDB_HANDLER_WRITE_ERROR;
//    }
//
//    char results[LARGE_BUFF_SIZE];
//    ssize_t bytes_read;
//
//    if (get_response_from_mdb(server, results, LARGE_BUFF_SIZE, &bytes_read) != MDB_HANDLER_SUCCESS) {
//        return MDB_HANDLER_READ_ERROR;
//    }
//
//    fprintf(stderr, "MDB Response [%zd bytes]:\n%s\n", bytes_read, results);
//
//    mdb_request_state status = validate_mdb_response(results, bytes_read);
//    char response[LARGE_BUFF_SIZE];
//
//    format_result_t fmt_result = format_html_response(results,
//                                                status,
//                                                response,
//                                                LARGE_BUFF_SIZE);
//    if (fmt_result != FORMAT_SUCCESS) {
//        return MDB_HANDLER_WRITE_ERROR;
//    }
//
//    ssize_t bytes_written;
//    utils_state write_result = write_socket(
//        server->activeClient->clntsock,
//        response,
//        strlen(response),
//        &bytes_written
//    );
//
//    return (write_result == UTILS_SUCCESS) ? MDB_HANDLER_SUCCESS : MDB_HANDLER_WRITE_ERROR;
//}

http_state process_lookup(WebServer *server, const char *query) {
    fprintf(stderr, "Processing lookup query: %s\n", query);

    ssize_t bytes_written;
    if (write_socket(server->mdbsock, query, strlen(query), &bytes_written) != UTILS_SUCCESS) {
        return HTTP_WRITE_ERROR;
    }

    char results[LARGE_BUFF_SIZE];
    mdbHandler_state mdb_result = get_response_from_mdb(server, results, sizeof(results));
    if (mdb_result != MDB_HANDLER_SUCCESS) {
        return HTTP_WRITE_ERROR;
    }

    // Determine status based on response content
    mdb_request_state status;
    if (strlen(results) == 0) {
        status = MDB_STATUS_EMPTY;
    } else if (strstr(results, "No matches") != NULL) {
        status = MDB_STATUS_NOT_FOUND;
    } else {
        status = MDB_STATUS_FOUND;
    }

    char response[LARGE_BUFF_SIZE];
    if (format_html_response(results, status, response, sizeof(response)) != FORMAT_SUCCESS) {
        return HTTP_FORMAT_ERROR;
    }

    if (write_socket(server->activeClient->clntsock, response, strlen(response), &bytes_written) != UTILS_SUCCESS) {
        return HTTP_WRITE_ERROR;
    }

    return HTTP_SUCCESS;
}
