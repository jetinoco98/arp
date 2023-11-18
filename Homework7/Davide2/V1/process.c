#include "headers/wrapfunc.h"
#include "wrapfunc.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// needed to prevent mutual access to this resource
sig_atomic_t id;

// Please note that both localtime and sprintf are not listed as safe for use inside
// signal handlers. In this case everything works as expected because one signal at 
// a time is handled. In other cases like the exercise 2 this would cause major issues.
// sprintf could be substituted by a combination of strcat, strcpy and a function that
// convert ints to strings, localtime by clock_gettime. I have not done that because it
// would be quite time consuming without major benefits in this case.
void sigHandler(int signum) {
    // buffer where to save data to save to file
    char toWrite[500];

    // getting current time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // printing date in a human friendly way
    sprintf(toWrite, "Process: %d - %d-%02d-%02d %02d:%02d:%02d\n",id, tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // writing in append to log file
    int fd = m_open("log.txt", O_WRONLY | O_APPEND);
    m_write(fd, toWrite, strlen(toWrite));
    m_close(fd);
}

int main(int argc, char *argv[]) {

    // getting id
    sscanf(argv[1], "%d", &id);
    
    // setting up the signal handler
    struct sigaction handle;
    // resetting in case of old values
    memset(&handle, 0, sizeof(handle));

    // assigning the handler
    handle.sa_handler = sigHandler;
    sigaction(SIGUSR1, &handle, NULL);

    // wasting time
    while (1) {
        usleep(500000 * id);
    }
    return EXIT_SUCCESS;
}
