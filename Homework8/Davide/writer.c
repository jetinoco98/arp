#include "headers/wrapfunc.h"
#include "constants.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>

int server_writer1[2];
int writer1_server[2];

int server_writer2[2];
int writer2_server[2];

int getCell(void* cellPtr, int id){
    char cell[MAX_CELL_SIZE/2];
    strncpy(cell, cellPtr+id*(MAX_CELL_SIZE/2)*sizeof(char), MAX_CELL_SIZE/2);

    return atoi(cell);
}

void setCell(void* cellPtr, int id, int val){
    char aux[4];
    sprintf(aux, "%d", val);
    strncpy((char*)cellPtr+id*(MAX_CELL_SIZE/2)*sizeof(char),aux,4);
}

int main(int argc, char *argv[]) {
    int stored = 0;
    int id = argv[1][0] - '0';
    printf("I am writer number %s\n", argv[1]);
    fflush(stdout);


    // init semaphores
    sem_t* sem1 = m_sem_open(SEM_CELL_1, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t* sem2 = m_sem_open(SEM_CELL_2, O_CREAT, S_IRUSR | S_IWUSR, 1);
    m_sem_init(sem1, 1, 1);
    m_sem_init(sem2, 1, 1);

    // Setup shared memory
    void* cellPtr;
    
    int cellsShm = m_shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    m_ftruncate(cellsShm, MAX_CELL_SIZE);

    // Note that here I am assuming that the maximum number is 999 because in this case
    // the maximum length of every numeber in the cell would be 3
    cellPtr = m_mmap(0, MAX_SHM_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, cellsShm, 0);


    if (id == 1) {
        sscanf(argv[2], "%d %d|%d %d", &server_writer1[0], &server_writer1[1],
               &writer1_server[0], &writer1_server[1]);

        close(server_writer1[1]);
        close(writer1_server[0]);
    } else if (id == 2) {
        sscanf(argv[2], "%d %d|%d %d", &server_writer2[0], &server_writer2[1],
               &writer2_server[0], &writer2_server[1]);

        close(server_writer2[1]);
        close(writer2_server[0]);
    }
    srandom(id * 123);

    while (true) {
        int toSend = random() % 100;
        char stringToSend[20];
        char response[11];

        printf("Asking to write in cell %d\n", id - 1);
        fflush(stdout);

        if (id == 1) {
            m_write(writer1_server[1], "W0", 3);
            m_read(server_writer1[0], response, 10);

            if (response[0] == 'y') {
                /* sprintf(stringToSend, "%d", toSend);
                m_write(writer1_server[1], stringToSend,
                      strlen(stringToSend) + 1); */

                m_sem_wait(sem1);
                setCell(cellPtr, 0, toSend);
                m_sem_post(sem1);

                printf("Writer 1 sent %d to server\n", toSend);
                fflush(stdout);

                stored++;
                printf("Writer 1 stored %d numbers\n", stored);
                fflush(stdout);
            }else{
                printf("Number has been rejected\n");
                fflush(stdout);
            }

        } else if (id == 2) {
            m_write(writer2_server[1], "W1", 3);
            m_read(server_writer2[0], response, 10);

            if (response[0] == 'y') {
                /* sprintf(stringToSend, "%d", toSend);
                m_write(writer2_server[1], stringToSend,
                      strlen(stringToSend) + 1); */

                m_sem_wait(sem2);
                setCell(cellPtr, 1, toSend);
                m_sem_post(sem2);

                printf("Writer 2 sent %d to server\n", toSend);
                fflush(stdout);

                stored++;
                printf("Writer 2 stored %d numbers\n", stored);
                fflush(stdout);
            }else{
                printf("Number has been rejected\n");
                fflush(stdout);
            }
        }
        sleep(3);
    }

    munmap(cellPtr, MAX_SHM_SIZE);
    m_sem_close(sem1);
    m_sem_close(sem2);
    m_sem_unlink(SEM_CELL_1);
    m_sem_unlink(SEM_CELL_2);
    return EXIT_SUCCESS;
}
