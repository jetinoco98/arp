// father.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "common.h"

int main() {
    int p1p_pipe[2];
    int p2p_pipe[2];

    pipe(p1p_pipe);
    pipe(p2p_pipe);

    pid_t p1_pid, p2_pid;

    // Fork first child (P1)
    if ((p1_pid = fork()) == 0) {
        close(p1p_pipe[1]);  // Close write end
        close(p2p_pipe[0]);  // Close read end
        execl("./child", "child", "P1", NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    }

    // Fork second child (P2)
    if ((p2_pid = fork()) == 0) {
        close(p1p_pipe[0]);  // Close read end
        close(p2p_pipe[1]);  // Close write end
        execl("./child", "child", "P2", NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    }

    // Close unused pipe ends in the parent
    close(p1p_pipe[0]);
    close(p2p_pipe[1]);

    char ch1 = 'A';
    char ch2 = 'B';
    int rate1 = 100000; // 0.1 seconds
    int rate2 = 150000; // 0.15 seconds

    // Send initial data to P1
    write(p1p_pipe[1], &ch1, sizeof(ch1));
    write(p1p_pipe[1], &rate1, sizeof(rate1));

    // Send initial data to P2
    write(p2p_pipe[0], &ch2, sizeof(ch2));
    write(p2p_pipe[0], &rate2, sizeof(rate2));

    // Wait for the children to finish
    waitpid(p1_pid, NULL, 0);
    waitpid(p2_pid, NULL, 0);

    return 0;
}
