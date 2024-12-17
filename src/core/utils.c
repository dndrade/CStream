#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include "utils.h"
#include "logger.h"
#include "models.h"


utils_state is_valid_port_number(int port) {
    return (port > 0 && port <= 65535) ? UTILS_SUCCESS : UTILS_INVALID_INPUT;
}

utils_state is_valid_path(const char *path) {
    if (!path || *path != '/' || strlen(path) >= PATH_MAX) {
        return UTILS_INVALID_INPUT;
    }
    return UTILS_SUCCESS;
}

utils_state is_valid_host(const char *host) {
    if (!host || !*host || strlen(host) >= HOST_NAME_MAX) {
        return UTILS_INVALID_INPUT;
    }
    return UTILS_SUCCESS;
}

utils_state ap_malloc(size_t size, void **ptr) {
    if (!ptr) { return UTILS_INVALID_INPUT; }

    *ptr = malloc(size);
    if (!*ptr) { return UTILS_ALLOC_FAILED; }

    memset(*ptr, 0, size);
    return UTILS_SUCCESS;
}

utils_state ap_strlloc(const char *str, char **result) {
    if (!str || !result) { return UTILS_INVALID_INPUT; }

    *result = malloc(strlen(str) + 1);
    if (!*result) { return UTILS_ALLOC_FAILED; }

    strcpy(*result, str);
    return UTILS_SUCCESS;
}

utils_state ap_open(const char *path, const char *mode, FILE **result) {
    if (!path || !mode || !result) { return UTILS_INVALID_INPUT; }

    *result = fopen(path, mode);
    return *result ? UTILS_SUCCESS : UTILS_IO_ERROR;
}

utils_state read_socket(int socket, char *buff, size_t size, ssize_t *bytes_read) {
    if (socket < 0 || !buff || !bytes_read) return UTILS_INVALID_INPUT;

    *bytes_read = read(socket, buff, size);
    return (*bytes_read >= 0) ? UTILS_SUCCESS : UTILS_IO_ERROR;
}

utils_state write_socket(int socket, const char *buff, size_t size, ssize_t *bytes_written) {
    if (socket < 0 || !buff || !bytes_written) { return UTILS_INVALID_INPUT; }

    size_t total = 0;
    while (total < size) {
        ssize_t written = write(socket, buff + total, size - total);
        if (written < 0) {
            *bytes_written = total;
            return UTILS_IO_ERROR;
        }
        total += written;
    }

    *bytes_written = total;
    return UTILS_SUCCESS;
}

utils_state close_socket(int socket) {
    if (socket < 0) { return UTILS_INVALID_INPUT; }
    return (close(socket) == 0) ? UTILS_SUCCESS : UTILS_IO_ERROR;
}


//utils_state format_query(const char *uri, char **result) {
//    // Find the key parameter
//    const char *key_start = strstr(uri, "key=");
//    if (!key_start) return UTILS_INVALID_INPUT;
//    key_start += 4; // Skip "key="
//
//    // Find end of the actual query (before HTTP/1.1)
//    const char *key_end = strstr(key_start, " HTTP");
//    if (!key_end) key_end = key_start + strlen(key_start);
//
//    size_t query_len = key_end - key_start;
//    *result = malloc(query_len + 2); // +1 for newline, +1 for null
//    if (!*result) return UTILS_ALLOC_FAILED;
//
//    strncpy(*result, key_start, query_len);
//    (*result)[query_len] = '\n';
//    (*result)[query_len + 1] = '\0';
//
//    return UTILS_SUCCESS;
//}

//static char formatted_query[LARGE_BUFF_SIZE];

utils_state format_query(const char *uri, char **result) {
    const char *key_start = strstr(uri, "key=");
    if (!key_start) return UTILS_INVALID_INPUT;
    key_start += 4; // Skip "key="

    const char *key_end = strstr(key_start, " HTTP");
    if (!key_end) key_end = key_start + strlen(key_start);

    size_t query_len = key_end - key_start;
    *result = malloc(query_len + 2); // +1 for newline, +1 for null
    if (!*result) return UTILS_ALLOC_FAILED;

    strncpy(*result, key_start, query_len);
    (*result)[query_len] = '\n';
    (*result)[query_len + 1] = '\0';

    return UTILS_SUCCESS;
}

utils_state format_path(const char *root, const char *path, char **result) {
    if (!root || !path || !result) return UTILS_INVALID_INPUT;

    char expanded_root[PATH_MAX];
    if (root[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) return UTILS_IO_ERROR;
        snprintf(expanded_root, PATH_MAX, "%s%s", home, root + 1);
        fprintf(stderr, "Expanded path: %s\n", expanded_root);
    } else {
        strncpy(expanded_root, root, PATH_MAX);
    }

    *result = malloc(PATH_MAX);
    if (!*result) return UTILS_ALLOC_FAILED;

    snprintf(*result, PATH_MAX, "%s%s", expanded_root, path);
    fprintf(stderr, "Final path: %s\n", *result);
    return UTILS_SUCCESS;
}


utils_state starts_with(const char *str, const char *prefix, int *result) {
    if (!str || !prefix || !result) return UTILS_INVALID_INPUT;

    *result = (strncmp(str, prefix, strlen(prefix)) == 0);
    return UTILS_SUCCESS;
}
