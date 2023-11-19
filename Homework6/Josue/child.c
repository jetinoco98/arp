// child.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "common.h"

volatile sig_atomic_t received_signal = 0;

void signal_handler(int signo) {
    if (signo == SIGUSR1) {
        received_signal = 1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [P1/P2]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *process_name = argv[1];
    int rate;

    // Set up signal handler
    signal(SIGUSR1, signal_handler);

    // Read initial data from the father
    char ch;
    read(STDIN_FILENO, &ch, sizeof(ch));
    read(STDIN_FILENO, &rate, sizeof(rate));

    while (1) {
        // Send the character to the father
        printf("%s sent: %c\n", process_name, ch);
        kill(getppid(), SIGUSR1);

        // Wait for the specified rate
        usleep(rate);

        // Wait until a signal is received from the father
        while (!received_signal) {
            usleep(1000);
        }

        // Reset the signal flag
        received_signal = 0;
    }

    return 0;
}
