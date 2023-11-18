#include "constants.h"
#include "headers/constants.h"
#include "wrapfunc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

void summon(char **programArgs) {
    execvp("konsole", programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}

void resetLogFile() {
    int fd = m_open("log.txt", O_WRONLY);
    // setting the first delmiter used for parsing by the watchdog
    m_write(fd, "--------------------------------\n", 33);
    m_close(fd);
}

int main(int argc, char *argv[]) {
    int watchdogPid;

    // resetting log file
    resetLogFile();

    // arguments initialization
    char *watchdogArgs[] = {"./watchdog", NULL};
    char *processArgs[] = {"./process", NULL, NULL, NULL};

    for (int i = 0; i < CHILDS_NUM; i++) {
        pid_t pid = m_fork();

        if (!pid) {
            // child
            if (i == 0) {
                // opening the watchdog in a separate window
                execvp("./watchdog", watchdogArgs);
                exit(0);
            } else {
                // create the processes and pass them as argument an id
                char aux[10];
                char aux1[10];
                sprintf(aux, "%d", i + 1);
                sprintf(aux1, "%d", watchdogPid);
                processArgs[1] = aux;
                processArgs[2] = aux1;
                execvp("./process", processArgs);
            }
        } else {
            // father

            // saving pid of watchdog
            if (i == 0) {
                printf("Summoned watchdog with pid %d\n", pid);
                fflush(stdout);
                watchdogPid = pid;
            } else {
                printf("Summoned child with pid %d\n", pid);
                fflush(stdout);
            }
        }
    }
    return EXIT_SUCCESS;
}
