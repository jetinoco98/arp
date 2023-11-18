#include "headers/constants.h"
#include "headers/wrapfunc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>

// initializing pipes
int server_reader1[2];
int reader1_server[2];

int server_reader2[2];
int reader2_server[2];

// function to retrieve the value inside shared memory
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
    char response[MAX_MESSAGE_LEN];
    int intResponse;
    char toSend[MAX_MESSAGE_LEN];
    int id = argv[1][0] - '0';
    void* cellPtr;

    // initialize shared memory
    char shmName[100] = SHM_NAME;
    int cellsShm = m_shm_open(shmName, O_CREAT | O_RDWR, 0666);
    m_ftruncate(cellsShm, MAX_CELL_SIZE);
    cellPtr = m_mmap(0, MAX_SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, cellsShm, 0);

    // initializing semaphores
    sem_t* sem1 = m_sem_open(SEM_CELL_1, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t* sem2 = m_sem_open(SEM_CELL_2, O_CREAT, S_IRUSR | S_IWUSR, 1);
    m_sem_init(sem1, 1, 1);
    m_sem_init(sem2, 1, 1);

    // Getting the input arguments
    if (id == 1) {
        sscanf(argv[2], "%d %d|%d %d", &server_reader1[0], &server_reader1[1],
               &reader1_server[0], &reader1_server[1]);
        close(server_reader1[1]);
        close(reader1_server[0]);
    } else if (id == 2) {
        sscanf(argv[2], "%d %d|%d %d", &server_reader2[0], &server_reader2[1],
               &reader2_server[0], &reader2_server[1]);
        close(server_reader2[1]);
        close(reader2_server[0]);
    }

    printf("I am reader number %s\n", argv[1]);
    fflush(stdout);

    while (true) {
        if (id == 1) {
            // sending the reading request to the server
            m_write(reader1_server[1], "R0", 3);
            printf("Sent reading request\n");
            
            // getting the response
            m_read(server_reader1[0], response, 2);
            if (response[0] == 'y') {
                printf("Read accepted\n");

                // in case the read was accepted now the read from
                // shared memory is performed
                m_sem_wait(sem1);
                intResponse = getCell(cellPtr, 0);
                m_sem_post(sem1);
                printf("Cell 0 contains %d\n", intResponse);
                fflush(stdout);
                

                // log entries are written
                int file = m_open("../logfiles/log_r.log", O_WRONLY|O_APPEND);
                sprintf(response, "Reader 1 read: %d\n", intResponse);
                m_write(file, response, strlen(response)+1);
                close(file);
            } else {
                printf("Read rejected\n");
                fflush(stdout);
            }
        } else {
            // sending reading request to the server
            m_write(reader2_server[1], "R1", 3);
            printf("Sent reading request\n");
            m_read(server_reader2[0], response, 2);
            if (response[0] == 'y') {
                printf("Read accepted\n");
                // in case the read was accepted now the read from
                // shared memory is performed
                fflush(stdout);
                m_sem_wait(sem2);
                intResponse = getCell(cellPtr, 1);
                m_sem_post(sem2);
                printf("Cell 0 contains %d\n", intResponse);
                fflush(stdout);

                // log entries are written
                int file = m_open("../logfiles/log_r.log", O_WRONLY|O_APPEND);
                sprintf(response, "Reader 2 read: %d\n", intResponse);
                m_write(file, response, strlen(response)+1);
                close(file);
                
            } else {
                printf("Read rejected\n");
                fflush(stdout);
            }
        }

        sleep(2);
    }
    munmap(cellPtr, MAX_SHM_SIZE);
    m_sem_close(sem1);
    m_sem_close(sem2);
    m_sem_unlink(SEM_CELL_1);
    m_sem_unlink(SEM_CELL_2);
    return EXIT_SUCCESS;
}
