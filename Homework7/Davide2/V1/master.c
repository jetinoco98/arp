#include "constants.h"
#include "headers/constants.h"
#include "wrapfunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>

void summon(char **programArgs) {
    execvp("konsole", programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}

void resetLogFile(){
    int fd = m_open("log.txt", O_WRONLY);
    // setting the first delmiter used for parsing by the watchdog
    m_write(fd, "--------------------------------\n", 33);
    m_close(fd);
}

int main(int argc, char *argv[]) {
    // string containing all the ids of the processes that the watchdog will
    // monitor. The format will be %d|%d|%d
    char pidsToMonitor[100];
    // array used to store pids of processes that the watchdog will monitor
    int wMpids[10];

    // arguments initialization
    char *watchdogArgs[] = {"konsole", "-e", "./watchdog", pidsToMonitor, NULL};
    char *processArgs[] = {"./process", NULL, NULL};

    // truncating logfile and setting first delimiter
    resetLogFile();

    for (int i = 0; i < CHILDS_NUM; i++) {
        pid_t pid = m_fork();

        if (!pid) {
            // child
            if (i == CHILDS_NUM - 1) {
                // if it's the last iteration create the watchdog
                char aux[10];
                sprintf(pidsToMonitor, "%d", wMpids[0]);
                // from 1 because the first is done in the previous line
                // this to allow the input string to be created dinamically
                for (int j = 1; j < CHILDS_NUM - 1; j++) {
                    sprintf(aux, "|%d", wMpids[j]);
                    strcat(pidsToMonitor, aux);
                }
                // opening the watchdog in a separate window
                summon(watchdogArgs);
            } else {
                // create the processes and pass them as argument an id
                char aux[10];
                sprintf(aux, "%d", i + 1);
                processArgs[1] = aux;
                execvp("./process", processArgs);
            }
        } else {
            // father
            printf("Summoned child with pid %d\n", pid);
            fflush(stdout);

            // saving pids of childs to send them to the watchdog
            wMpids[i] = pid;
        }
    }

    return EXIT_SUCCESS;
}
