CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE

all: signal_handler.o
	$(CC) $(CFLAGS) $^ -o signal_handler

signal_handler.o: signal_handler.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f *.o *.h.gch signal_handler
	