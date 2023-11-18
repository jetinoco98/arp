#include "constants.h"
#include "headers/wrapfunc.h"
#include "wrapfunc.h"
#include <curses.h>
#include <fcntl.h>
#include <ncurses.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int main(int argc, char *argv[]) {
    int childsPid[CHILDS_NUM - 1];

    // getting the pids
    sscanf(argv[1], "%d|%d|%d", &childsPid[0], &childsPid[1], &childsPid[2]);

    // initializing ncurses
    initscr();
    // remove the cursor
    curs_set(0);
    // enabling ncurses colors
    start_color();
    // use default terminal colors. This is used to get the default background of 
    // the terminal
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
    while (1) {
        // sinding singals to the processes
        for (int i = 0; i < CHILDS_NUM - 1; i++) {
            kill(childsPid[i], SIGUSR1);
        }
        
        // getting size of the terminal to display windows correctly
        getmaxyx(stdscr, height, width);

        sleep(5);
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

        // for how the logfile is structured each line has a
        // fixed length of 32 +'\n' characters so we can exploit this
        char content[33];
        // bool array where to save if a process wrote to log
        bool reported[3] = {false, false, false};

        FILE *f;
        f = fopen("log.txt", "r");
        // the read is performed from the end of the file
        fseek(f, -33, SEEK_END);
        int counter = 0;
        // the files is read line by line from the end
        while (fread(content, 33, 1, f)) {
            fseek(f, -33 * (++counter), SEEK_END);
            // if a - is present in position 0 then we reached
            // the last check position
            if (content[0] == '-')
                break;
            // else there is the id of the child
            reported[content[9] - '1'] = true;
        }
        fclose(f);
        
        // we set a line of --- to indicate that the last check occoured here
        int fd = m_open("log.txt", O_WRONLY | O_APPEND);
        m_write(fd, "--------------------------------\n", 33);
        m_close(fd);

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
