#define _POSIX_C_SOURCE
#define _DEFAULT_SOURCE
//#define _BSD_SOURCE if compiled on zid
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> //gives access to the POSIX OS API -> for STDOUT_FILENO for example

/*
I watched this tutorial and afterwards worked on my solution, there might be some similarities.
https://www.youtube.com/watch?v=83M5-NPDeWs
*/

void handler(int sig_num) {
    if(sig_num == SIGINT) {
        write(STDOUT_FILENO, "caught SIGINT\n", 14); //write(output-stream, STRING, STRING_LENGTH)
    } else if (sig_num == SIGSTOP) {
        write(STDOUT_FILENO, "\ncaught SIGSTOP\n", 14);
    } else if(sig_num == SIGCONT) {
        write(STDOUT_FILENO, "\ncaught SIGCONT\n", 16);
    } else if(sig_num == SIGKILL) {
       write(STDOUT_FILENO, "caught SIGKILL\n", 14);
    } else {
        write(STDOUT_FILENO, "unknown signal received\n", 23);
    }
}


int main() {
    struct sigaction sa;
    sa.sa_handler = handler;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGCONT, &sa, NULL);
    //still here for showing that sigstop and sigkill can't be caught
    //sigaction(SIGSTOP, &sa, NULL);
    //sigaction(SIGKILL, &sa, NULL);
    
    while(1) {
        printf("running %d\n", getpid()); //getpid gets the id of the process
        usleep(3000000); //usleep works with microseconds
    }

}