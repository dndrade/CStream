#ifndef MSG_H
#define MSG_H


#include <stdio.h>


#define MSG_ALLOC_ERROR "%s allocation failed"
#define MSG_CLOSE_ERROR "Failed to close %s"
#define MSG_CONNECT_ERROR "Failed to connect to %s"
#define MSG_INVALID_ERROR "Invalid %s"
#define MSG_PATH_ERROR "Path %s"
#define MSG_SOCKET_ERROR "%s socket operation failed"
#define MSG_METHOD_ERROR "Only GET method supported"
#define MSG_URI_ERROR "URI must start with '/'"


#define LOG_SERVER_START "Server started on port %d"
#define LOG_SERVER_SHUTDOWN "Server shutting down"
#define LOG_CLEANUP "Cleaning up %s resources"

#define LOG_CLIENT_CONNECT "Client connected from %s"
#define LOG_CLIENT_DISCONNECT "Client disconnected: %s"

#define LOG_MDB_CONNECT "Connected to MDB server"
#define MSG_MDB_NOT_FOUND "No results found for: %s"
#define MSG_MDB_ERROR "MDB error: %s"

#define CHECK_NULL(ptr, ret) if (!(ptr)) return (ret)
#define CHECK_RANGE(val, min, max, ret) if ((val) < (min) || (val) > (max)) return (ret)


#endif
