#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "debug.h"


void read_all(int fd, void* buf, ssize_t size){
    char *current_buf = (char *) buf;
    DEBUG_PRINT_2("reading %zu bytes\n", size);
    while(size > 0){
        ssize_t read_bytes = read(fd, current_buf, size);
        size -= read_bytes;
        DEBUG_PRINT_2("read %zu bytes, %zu remaining\n", read_bytes, size);
        current_buf += read_bytes;
    }
}

int write_all(int fd, void const* data, size_t size){
    DEBUG_PRINT_2("writing %zu bytes\n", size);
    while(size > 0){
        ssize_t writtenData = write(fd, data, size);
        DEBUG_PRINT_2("wrote %zu bytes\n", writtenData);
        if(writtenData <= 0){
            fprintf(stderr, "error writing data");
            return EXIT_FAILURE;
        }
        
        size -= writtenData;
    }
    return EXIT_SUCCESS;
}