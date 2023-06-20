CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2 -g -pthread
LDFLAGS = -pthread

TARGETS = task

.PHONY:
all: $(TARGETS)

.PHONY:
clean:
	$(RM) $(TARGETS) *.o

thread_pool.o: thread_pool.c thread_pool.h
