#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include "models.h"


utils_state is_valid_port_number(int port);
utils_state is_valid_path(const char *path);
utils_state is_valid_host(const char *host);

utils_state ap_malloc(size_t size, void **ptr);
utils_state ap_strlloc(const char *str, char **result);
utils_state ap_open(const char *path, const char *mode, FILE **result);

utils_state read_socket(int socket, char *buff, size_t size, ssize_t *bytes_read);
utils_state write_socket(int socket, const char *buff, size_t size, ssize_t *bytes_written);
utils_state close_socket(int socket);

utils_state format_path(const char *root, const char *path, char **result);
utils_state format_query(const char *uri, char **result);
utils_state starts_with(const char *str, const char *prefix, int *result);


#endif