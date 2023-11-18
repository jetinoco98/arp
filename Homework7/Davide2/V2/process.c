#include "headers/wrapfunc.h"
#include "wrapfunc.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // getting watchdog id
    int watchdog;
    sscanf(argv[2], "%d", &watchdog);

    // getting id
    int id;
    sscanf(argv[1], "%d", &id);

    // In the next lines the previous implementation of the process can 
    // be seen. This was more in line with the exercise but was more annoying
    // to kill the different processes. So a simpler imlpementation that
    // does the same thing is proposed
    
    // one process will represent the main process and
    // the other will be the responsible to send data to
    // the watchdog
    //
    /* int pid = m_fork();
    if (!pid) {
        // child
        while (1) {
            sleep(3+id);
            kill(watchdog, SIGUSR1);
        }
    } else {
        // father
        //  wasting time
        while (1) {
            usleep(500000 * id);
        }
    } */

    // Note that if the signals arrive at the same time the watchdog can
    // become unreliable. So id to defer a bit the messages is added to 
    // the sleep time of the process
    while (1) {
        sleep(2+id);
        kill(watchdog, SIGUSR1);
    }
    return EXIT_SUCCESS;
}
