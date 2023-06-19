/* Disclosure: The code provided herein contains a code template
that was derived from the repository "Fabi12345678910/c_templates." */
#define _POSIX_C_SOURCE 200809L
#include "common.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>

#include "common.h"
#include "write_read_all.h"

#define TARGET_ADRESS "::1"

int client(uint16_t port){ //TODO expand by client data
        struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port);
    if(inet_pton(AF_INET6, TARGET_ADRESS, &addr.sin6_addr) != 1){
        fprintf(stderr, "error converting address\n");
        return EXIT_FAILURE;
    }

    int client_socket = socket(AF_INET6, SOCK_STREAM, 0);
    if (client_socket == -1){
        fprintf(stderr, "error creating socket\n");
        return EXIT_FAILURE;
    }

    if(connect(client_socket,(struct sockaddr *) &addr, sizeof(addr))){
        fprintf(stderr, "error connecting to server\n");
        close(client_socket);
        return EXIT_FAILURE;
    }

    struct clientMessage message;
    //TODO fill in the components of message
    message.dummy = 1;

    write_all(client_socket, &message, sizeof(message));
    return EXIT_SUCCESS;
}

int main(int argc, char const *argv[])
{
    //do some shit to read in the arguments
    if (argc != 3){
        fprintf(stderr, "usage: %s <port> <username>\n", argv[0]);
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

    return client(port);
}

