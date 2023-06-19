CPPFLAGS += -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE -D_BSD_SOURCE
CFLAGS += -std=c11 -Wall -Wextra -pedantic -pthread -g -O3
LDFLAGS += -pthread

# Add the names of your executables here
TARGETS = server client

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	$(RM) $(TARGETS) *.o

%: %.o
	gcc -o $* $(CFLAGS) $(CPPFLAGS) $*.o

%.out: %.c
	gcc -o $* $*.c

%.so: %.o
	gcc -shared $(CFLAGS) $(CPPFLAGS) -o $*.so $*.o

%.o: %.c
	gcc -c $(CFLAGS) $(CPPFLAGS) $*.c

server.o: server.c common.h
	gcc -c $(CFLAGS) $(CPPFLAGS) $*.c

client.o: client.c common.h
	gcc -c $(CFLAGS) $(CPPFLAGS) $*.c