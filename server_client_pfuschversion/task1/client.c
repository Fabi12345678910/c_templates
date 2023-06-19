#define _POSIX_C_SOURCE 200809L

#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>

#define MSG_SIZE 128

typedef struct message {
	//TODO: implement message fields
} message_t;

int main(int argc, char const* argv[]) {
	// Check if the right amount of arguments is given
	if(argc < 3 || argc > 5) {
		printf("Usage: %s <port> <command> <sensor-id> <temperature>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Parse port
	uint16_t port;
    char *end;
    long input_port = strtol(argv[1], &end, 10);
    if (*end != '\0' || input_port < 0 || input_port > UINT16_MAX){
        fprintf(stderr, "error decoding port\n");
        return EXIT_FAILURE;
    }
    port = (uint16_t) input_port;

	message_t msg;
	
	// open socket
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1) {
		perror("creating socket failed");
		return EXIT_FAILURE;
	}

	// init internet socket address
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	// connect to server
	if(connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
		perror("connecting failed");
		return EXIT_FAILURE;
	}

	// send command to server
	if(write(fd, (char*)&msg, sizeof(msg)) == -1) {
		perror("writing failed");
		return EXIT_FAILURE;
	}


	//TODO: optional if reading is required
	/*
		char buffer[100] = { 0 };
		// read answer from server
		if(read(fd, buffer, sizeof(buffer)) == -1) {
			perror("reading failed");
			return EXIT_FAILURE;
		}

		printf("%s\n", buffer);
	*/
	return EXIT_SUCCESS;
}
