#include "constants.h"
#include "headers/wrapfunc.h"
#include "wrapfunc.h"
#include <bits/time.h>
#include <curses.h>
#include <fcntl.h>
#include <ncurses.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// creating the windows where the status of the processes will be displayed
WINDOW *windowSetup(int height, int width, int starty, int startx, int pid) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0); /* 0, 0 gives default characters
                           * for the vertical and horizontal
                           * lines			*/
    mvwprintw(local_win, 0, 1, "%d", pid);
    wrefresh(local_win); /* Show that box 		*/
    return local_win;
}

// destroying the window to have refreshing of the interface
void destroy_win(WINDOW *local_win) {
    // removing borders
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    // removing content
    werase(local_win);
    // displaying changes
    wrefresh(local_win);
    // deleting the window
    delwin(local_win);
}

sig_atomic_t lastp[3] = {0, 0, 0};
sig_atomic_t childsPid[3] = {0, 0, 0};
sig_atomic_t childNum = 0;
void int2str(int num, char str[100]) {
    int numC = 0;
    char aux[100];
    while (num != 0) {
        int c = num % 10;
        aux[numC] = c + '0';
        num /= 10;
        numC++;
    }
    numC--;
    int index = 0;
    str[numC] = '\0';
    for (; numC >= 0; numC--) {
        str[index++] = aux[numC];
    }
}

void signalHandel(int sig, siginfo_t *sigInfo, void *context) {
    int id = sigInfo->si_pid;

    bool found = false;
    for (int i = 0; i < CHILDS_NUM - 1; i++) {
        if (id == childsPid[i]) {
            found = true;
            struct timespec time;
            clock_gettime(CLOCK_MONOTONIC_RAW, &time);
            lastp[i] = time.tv_sec;
        }
    }
    if (!found) {
        childsPid[childNum++] = id;
    }

    // buffer where to save data to save to file
    char toWrite[500];
    char aux[100];

    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    // printing date in a human friendly way
    // NOTE sprintf cannot be used inside a signal handler
    strcpy(toWrite, "Process: ");
    int2str(id, aux);
    strcat(toWrite, aux);
    strcat(toWrite, "\n");

    // writing in append to log file
    int fd = m_open("log.txt", O_WRONLY | O_APPEND);
    m_write(fd, toWrite, strlen(toWrite));
    m_close(fd);
}

int main(int argc, char *argv[]) {
    // setting up signal handling
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO | SA_NODEFER | SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = &signalHandel;
    sigaction(SIGUSR1, &sa, NULL);

    // initializing ncurses
    initscr();
    // remove the cursor
    curs_set(0);
    // enabling ncurses colors
    start_color();
    // use default terminal colors. This is used to get the default background
    // of the terminal
    use_default_colors();

    // Setting the pairs needed for coloring. The firstt color is
    // foreground and the second background. Note -1 means use the default
    init_pair(1, COLOR_GREEN, -1);
    init_pair(2, COLOR_RED, -1);

    // creating windows to desplay status
    WINDOW *p1, *p2, *p3;
    int height, width;
    getmaxyx(stdscr, height, width);
    p1 = windowSetup(3, 9, height / 2 - 1, width / 2 - 15, childsPid[0]);
    p2 = windowSetup(3, 9, height / 2 - 1, width / 2 - 5, childsPid[1]);
    p3 = windowSetup(3, 9, height / 2 - 1, width / 2 + 5, childsPid[2]);
    int count = 0;
    while (1) {
        // getting size of the terminal to display windows correctly
        getmaxyx(stdscr, height, width);

        sleep(3);
        // bool array where to save if a process wrote to log
        bool reported[3] = {true, true, true};

        struct timespec time;
        clock_gettime(CLOCK_MONOTONIC_RAW, &time);
        int now = time.tv_sec;
        move(0, 0);
        printw("Now: %d\n", now);
        for (int i = 0; i < 3; i++) {
            printw("Sig time: %d\n", lastp[i]);
            if (now - lastp[i] > 10) {
                reported[i] = false;
            }
        }
        printw("Run: %d\n", count++);
        // clearing the screen
        refresh();

        // removing old windows
        destroy_win(p1);
        destroy_win(p2);
        destroy_win(p3);

        // creating updated windows
        p1 = windowSetup(3, 9, height / 2 - 1, width / 2 - 15, childsPid[0]);
        p2 = windowSetup(3, 9, height / 2 - 1, width / 2 - 5, childsPid[1]);
        p3 = windowSetup(3, 9, height / 2 - 1, width / 2 + 5, childsPid[2]);

        // setting processes status
        if (reported[0] == true) {
            wattron(p1, COLOR_PAIR(1));
            mvwprintw(p1, 1, 1, "RUNNING");
            wattroff(p1, COLOR_PAIR(1));
        } else {
            wattron(p1, COLOR_PAIR(2));
            mvwprintw(p1, 1, 1, "KILLED");
            wattroff(p1, COLOR_PAIR(2));
        }

        if (reported[1] == true) {
            wattron(p2, COLOR_PAIR(1));
            mvwprintw(p2, 1, 1, "RUNNING");
            wattroff(p2, COLOR_PAIR(1));
        } else {
            wattron(p2, COLOR_PAIR(2));
            mvwprintw(p2, 1, 1, "KILLED");
            wattroff(p2, COLOR_PAIR(2));
        }

        if (reported[2] == true) {
            wattron(p3, COLOR_PAIR(1));
            mvwprintw(p3, 1, 1, "RUNNING");
            wattroff(p3, COLOR_PAIR(1));
        } else {
            wattron(p3, COLOR_PAIR(2));
            mvwprintw(p3, 1, 1, "KILLED");
            wattroff(p3, COLOR_PAIR(2));
        }

        // refreshing after changes
        wrefresh(p1);
        wrefresh(p2);
        wrefresh(p3);
    }
    endwin();
    return EXIT_SUCCESS;
}
