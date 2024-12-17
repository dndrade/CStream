/*
 * [ lab 7 ]
 * File: http-server.h
 * UNI: tl3305
 * Name: Thamires Lopes de Andrade
 * Date: 12-10-2024
 * Description: function and struct declarations
 * 
 */

#ifndef SERVER_H
#define SERVER_H


#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include "models.h"
#include "utils.h"
#include "msg.h"
#include "error_handler.h"
#include "http_handler.h"
#include "mdb_handler.h"


#define REQUEST_BUFF_SIZE 8192

server_state init_server(char *web_root, const char *ports[],  char *mdb_host, WebServer **server);
server_state run_server(WebServer *server);
server_state cleanup_server(WebServer *server);

#endif
