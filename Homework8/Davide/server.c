#include "headers/constants.h"
#include "headers/wrapfunc.h"
#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

// setting up cells
void ncursesSetup(WINDOW **topLeft,WINDOW **topRight,WINDOW **bottomLeft,WINDOW **bottomRight,WINDOW **logger){
    const int cellDim = 6;
    int initPos[10] = {
        (LINES/2 - cellDim)/2,
        COLS/2 - cellDim,
        (LINES/2 - cellDim)/2,
        COLS/2,
        LINES/4,
        COLS/2 - cellDim,
        LINES/4,
        COLS/2,
        LINES/2,
        COLS/4
    };
    *topLeft = create_newwin(cellDim/2, cellDim, initPos[0], initPos[1]);
    *topRight = create_newwin(cellDim/2, cellDim, initPos[2], initPos[3]);
    *bottomLeft = create_newwin(cellDim/2, cellDim, initPos[4], initPos[5]);
    *bottomRight = create_newwin(cellDim/2, cellDim, initPos[6], initPos[7]);
    *logger = create_newwin(LINES/2, COLS/2, initPos[8], initPos[9]);

    mvwprintw(*topLeft,1,2,"C0");
    wrefresh(*topLeft);
    mvwprintw(*topRight,1,2,"C1");
    wrefresh(*topRight);
}

// function that handles the writing of a new log entry in the display
void loggerAdd(int* loggerDim, WINDOW** logger, char loggerStrings[MAX_LOGGER_LEN][MAX_MESSAGE_LEN], const char string[MAX_MESSAGE_LEN]){
    // Here I am making sure that the lines don't go over the limit of the window
    if(*loggerDim < MAX_LOGGER_LEN)(*loggerDim)++;
    for(int i = *loggerDim; i >= 0; i--){
       //dest - src
       strcpy(loggerStrings[i+1], loggerStrings[i]);
    }

    strcpy(loggerStrings[0], string);

    // here the lines are actually displayed making sure to be centered
    for(int i = 0; i < *loggerDim && i < getmaxy(*logger)-2; i++){
        mvwprintw(*logger, i+1, (getmaxx(*logger)-strlen(string))/2, "%s", loggerStrings[i]);
    }
    wrefresh(*logger);
}

// function to get the cell value
int getCell(void* cellPtr, int id){
    char cell[MAX_CELL_SIZE/2];
    // the shared memory is 8 bytes and in here I am using pointer aritmetics
    // to take the right 4 bytes from it
    strncpy(cell, cellPtr+id*(MAX_CELL_SIZE/2)*sizeof(char), MAX_CELL_SIZE/2);

    return atoi(cell);
}

void setCell(void* cellPtr, int id, int val){
    char aux[4];
    sprintf(aux, "%d", val);
    // Also here as in getCell pointer aritmetics is used to get the right cell
    strncpy((char*)cellPtr+id*(MAX_CELL_SIZE/2)*sizeof(char),aux,4);
}

int main(int argc, char *argv[]) {
    // init semaphores
    sem_t* sem1 = m_sem_open(SEM_CELL_1, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t* sem2 = m_sem_open(SEM_CELL_2, O_CREAT, S_IRUSR | S_IWUSR, 1);
    m_sem_init(sem1, 1, 1);
    m_sem_init(sem2, 1, 1);

    // Setup shared memory
    void* cellPtr;
    char shmName[100] = SHM_NAME;
    
    int cellsShm = m_shm_open(shmName, O_CREAT | O_RDWR, 0666);
    m_ftruncate(cellsShm, MAX_CELL_SIZE);
    
    // Note that here I am assuming that the maximum number is 999 because in this casevo
    // the maximum length of every numeber in the cell would be 3
    cellPtr = m_mmap(0, MAX_SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, cellsShm, 0);
    sprintf(cellPtr, "%s", "00000000");

    // masking sig_winch that causes the program to crash if the window is resized
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGWINCH);
    sigprocmask(SIG_BLOCK, &mask, NULL);


    char loggerStrings[MAX_LOGGER_LEN][MAX_MESSAGE_LEN];
    char logmsg[MAX_MESSAGE_LEN+30];// it needs to accomodate also strings like "RECEIVED" 
    int loggerDim = 0;
    //
    //ncerses initialization
    initscr();
    cbreak();

    WINDOW *topLeft, *topRight, *bottomLeft, *bottomRight, *logger;
    ncursesSetup(&topLeft, &topRight, &bottomLeft, &bottomRight, &logger);

    int lastReadValue[] = {-1, -1};

    char receivedMsg[MAX_MESSAGE_LEN];
    char toSend[MAX_MESSAGE_LEN];

    int server_reader1[2];
    int reader1_server[2];

    int reader2_server[2];
    int server_reader2[2];

    int server_writer1[2];
    int writer1_server[2];

    int server_writer2[2];
    int writer2_server[2];

    fd_set master, reading;


    // setting up pipes
    sscanf(argv[1], "%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d",
           &server_reader1[0], &server_reader1[1], &server_reader2[0],
           &server_reader2[1], &server_writer1[0], &server_writer1[1],
           &server_writer2[0], &server_writer2[1], &reader1_server[0],
           &reader1_server[1], &reader2_server[0], &reader2_server[1],
           &writer1_server[0], &writer1_server[1], &writer2_server[0],
           &writer2_server[1]);
    close(writer1_server[1]);
    close(writer2_server[1]);
    close(reader1_server[1]);
    close(reader2_server[1]);

    FD_ZERO(&master);
    FD_ZERO(&reading);

    FD_SET(writer1_server[0], &master);
    FD_SET(writer2_server[0], &master);
    FD_SET(reader1_server[0], &master);
    FD_SET(reader2_server[0], &master);

    int fds[4];
    fds[0] = writer1_server[0];
    fds[1] = writer2_server[0];
    fds[2] = reader1_server[0];
    fds[3] = reader2_server[0];

    // searching for max_fd
    int max_fd = -1;
    for(int i = 0; i < 4; i++){
        if(fds[i] > max_fd){
            max_fd = fds[i];
        }
    }
    int runs = 0;
    int processes = 4;
    while (1) {
        runs++;
        reading = master;
        m_select(max_fd + 1, &reading, NULL, NULL, NULL);
        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &reading)) {
                m_read(i, receivedMsg, MAX_MESSAGE_LEN);
                if (i == writer1_server[0]) {
                    sprintf(logmsg, "RECEIVED %s from writer 1", receivedMsg);
                    loggerAdd(&loggerDim, &logger, loggerStrings, logmsg);

                    m_sem_wait(sem1);
                    int cell1 = getCell(cellPtr, 0);
                    m_sem_post(sem1);

                    m_sem_wait(sem2);
                    int cell2 = getCell(cellPtr, 1);
                    m_sem_post(sem2);

                    if (cell1 <= cell2) {
                        m_write(server_writer1[1], "y", 2);
                    } else {
                        m_write(server_writer1[1], "n", 2);
                    }

                    // refreshing data on screen
                    mvwprintw(bottomLeft, 1, 1, "%d", cell1);
                    mvwprintw(bottomRight, 1, 1, "%d", cell2);
                    wrefresh(bottomLeft);
                    wrefresh(bottomRight);

                } else if (i == writer2_server[0]) {
                    sprintf(logmsg, "RECEIVED %s from writer 2", receivedMsg);
                    loggerAdd(&loggerDim, &logger, loggerStrings, logmsg);

                    m_sem_wait(sem1);
                    int cell1 = getCell(cellPtr, 0);
                    m_sem_post(sem1);

                    m_sem_wait(sem2);
                    int cell2 = getCell(cellPtr, 1);
                    m_sem_post(sem2);

                    if (cell1 >= cell2) {
                        m_write(server_writer2[1], "y", 2);
                    } else {
                        m_write(server_writer2[1], "n", 2);
                    }

                    // refreshing data on screen
                    mvwprintw(bottomLeft, 1, 1, "%d", cell1);
                    mvwprintw(bottomRight, 1, 1, "%d", cell2);
                    wrefresh(bottomLeft);
                    wrefresh(bottomRight);
                } else if (i == reader1_server[0]) {
                    sprintf(logmsg, "RECEIVED %s from reader 1", receivedMsg);
                    loggerAdd(&loggerDim, &logger, loggerStrings, logmsg);

                    m_sem_wait(sem1);
                    int cell1 = getCell(cellPtr, 0);
                    m_sem_post(sem1);

                    // if the value is changed since last read then allow reading
                    if (lastReadValue[0] != cell1) {
                        lastReadValue[0] = cell1;
                        m_write(server_reader1[1], "y", 2);
                    }else{
                        m_write(server_reader1[1],"n",2);
                    }
                } else if (i == reader2_server[0]) {
                    sprintf(logmsg, "RECEIVED %s from reader 2", receivedMsg);
                    loggerAdd(&loggerDim, &logger, loggerStrings, logmsg);

                    m_sem_wait(sem1);
                    int cell2 = getCell(cellPtr, 1);
                    m_sem_post(sem1);

                    // if the value is changed since last read then allow reading
                    if (lastReadValue[1] != cell2) {
                        lastReadValue[1] = cell2;
                        m_write(server_reader2[1], "y", 2);
                    }else{
                        m_write(server_reader2[1],"n",2);
                    }
                }
            }
        }
    }
    // cleaning up
    endwin();
    munmap(cellPtr, MAX_SHM_SIZE);
    m_sem_close(sem1);
    m_sem_close(sem2);
    m_sem_unlink(SEM_CELL_1);
    m_sem_unlink(SEM_CELL_2);
    return EXIT_SUCCESS;
}
