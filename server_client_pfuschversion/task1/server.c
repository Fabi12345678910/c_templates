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

//invariant: sensor name is always 10 characters long
typedef struct message {
	//TODO: add required fields
} message_t;

typedef struct arg_struct {
	pthread_mutex_t* mutex;
	int new_socket_fd;
} arg_struct;



void* client(void* args) {
	arg_struct* arg = (arg_struct*)args;
	pthread_mutex_t* mutex = arg->mutex;

	char buffer[sizeof(message_t)] = {0};
	int bytes_read = read(arg->new_socket_fd, buffer, sizeof(buffer));
	if(bytes_read == -1) {
		perror("reading from socket failed");
		return NULL;
	}


	message_t* msg = (message_t*)buffer;

	return NULL;
}

int main(int argc, char const* argv[]) {
	if(argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Parse port
	char* endptr;
	
	uint16_t port;
	long input_port = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || input_port < 0 || input_port > UINT16_MAX){
        fprintf(stderr, "error decoding port\n");
        return EXIT_FAILURE;
    }
    port = (uint16_t) input_port;


	// init mutex
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// bind socket to address
	int bound = bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
	if(bound == -1) {
		perror("binding socket failed");
		return EXIT_FAILURE;
	}

	// listen for connections on a socket (queue length 10)
	int listening = listen(fd, 10);
	if(listening == -1) {
		perror("listening on socket failed");
		return EXIT_FAILURE;
	}

	printf("Listening on port %d!\n", port);


	//TODO: join threads
	for(int i = 0; i < 5; i++) {
		//pthread_join(threads[i], NULL);
	}

	//TODO: free mutex and recources
	pthread_mutex_destroy(&mutex);
	
	return EXIT_SUCCESS;
}
