CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pthread

all: math_server math_client

math_server: math_server.c
	$(CC) $(CFLAGS) -o $@ $^

math_client: math_client.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) -f math_server
	$(RM) -f math_client