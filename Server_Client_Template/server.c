//this file is based on my template for a simple client server connection,
//    which is based on exercise09/task2

#define _POSIX_C_SOURCE 200809L
#include "common.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define DEBUG_LEVEL 1

#include "debug.h"
#include "common.h"
#include "write_read_all.h"

#define CONNECTIONS_BEFORE_SHUTDOWN 5

struct server_data
{
    pthread_mutex_t mutex_data_access;
};

struct connection_data{
    int fd_client_socket;
};

struct handler_data{
    struct server_data *server_data;
    struct connection_data connection_data;
};

void* connection_handler(void *);

int server(uint16_t server_port){
    int server_socket = socket(AF_INET6, SOCK_STREAM, 0);
    if (server_socket == -1){
        fprintf(stderr, "error creating socket\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(server_port);

    DEBUG_PRINT_1("binding socket\n");
    if(bind(server_socket,(struct sockaddr *) &addr, sizeof(addr))){
        fprintf(stderr, "error binding socket\n");
        close(server_socket);
        return EXIT_FAILURE;
    }

    if(listen(server_socket, 1)){
        fprintf(stderr, "error setting socket to listening\n");
        close(server_socket);
        return EXIT_FAILURE;
    }
    printf("Listening on port %hu\n", server_port);

    //could perhaps be done inside a listener thread:
    pthread_t threads[CONNECTIONS_BEFORE_SHUTDOWN];
    struct handler_data handler_storage[CONNECTIONS_BEFORE_SHUTDOWN];
    struct server_data server_data;

    for(size_t i = 0; i < CONNECTIONS_BEFORE_SHUTDOWN; i++){
        int clientSock = accept(server_socket, NULL, NULL);
        DEBUG_PRINT_1("accepted new connection\n");
        if(clientSock == -1){
            perror("error accepting new client");
        }else{
            handler_storage[i].connection_data.fd_client_socket = clientSock;
            handler_storage[i].server_data = &server_data;
            pthread_create(&threads[i], NULL, connection_handler, &handler_storage[i]);
        }
    }

    for(size_t i = 0; i < CONNECTIONS_BEFORE_SHUTDOWN; i++){
        pthread_join(threads[i], NULL);
    }

    return EXIT_SUCCESS;
}

void* connection_handler(void * arg){
    struct handler_data *handler_data = arg;

    //read single message
    struct clientMessage msg;
    read_all(handler_data->connection_data.fd_client_socket, &msg, sizeof(msg));

    return NULL;
}

int main(int argc, char const *argv[])
{
    //assume single argument <port>
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port;
    char *end;
    long input_port = strtol(argv[1], &end, 10);
    if (*end != '\0' || input_port < 0 || input_port > UINT16_MAX){
        fprintf(stderr, "error decoding port\n");
        return EXIT_FAILURE;
    }
    port = (uint16_t) input_port;

    return server(port);
}
