#include <semaphore.h>
#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include "include/constants.h"
#include <sys/mman.h>

int main()
{
 
    int i;
    int res;

    // Temporary char arrays for the command line arguments of each philospher
    char cl_arg[8];

    // Create state vector
    int state[N_PHILOSOPHERS];
    int shared_seg_size = (1 * sizeof(state));

    // Create shared memory for state
    int shmfd  = shm_open(SHM_PATH, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0)
    {
        perror("shm_open");
        return -1;
    }
    // truncate size of shared memory
    ftruncate(shmfd, shared_seg_size);
    // map pointer
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    //Create semaphores
    char * sem_paths[N_PHILOSOPHERS];
    sem_t * sem_ids[N_PHILOSOPHERS]; 
    sem_t * state_mutex;


    char tmp_path[80];
 
    // semaphores for each philosopher (not for each fork)
    for (i = 0; i < N_PHILOSOPHERS; i++)
    {
        sprintf(tmp_path, SEMAPHORE_PATH_FORMAT, i);
        sem_paths[i] = tmp_path;
        sem_ids[i] = sem_open(sem_paths[i], O_CREAT, S_IRUSR | S_IWUSR, 1);
        sem_init(sem_ids[i], 1, 0); //initialize semaphores to zero because test will post them
    }

    // Mutex for accessing the state shared memory
    state_mutex = sem_open(STATE_MUTEX_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_init(state_mutex, 1, 1);

    // create/clear log file
    FILE * fp;
    fp = fopen(LOG_FILE, "w");
    fclose(fp);


    // pid for each philospher
    pid_t pids[N_PHILOSOPHERS];

    // Create an instance of philospher with the correct philosopher number
    for (i = 0; i < N_PHILOSOPHERS; i++) 
    {
        pids[i] = fork();
        if (pids[i] < 0) {
        perror("Fork");
        return -1;
        }

        if (pids[i] == 0) {
            sprintf(cl_arg, "%d", i); // create a string of the philosopher number
            char * arg_list[] = { "konsole", "-e", "./philosopher", cl_arg, NULL };
            execvp("konsole", arg_list);
            return 0;
        }
    }
 
    // wait for all processes to end
    for (i = 0; i < N_PHILOSOPHERS; i++)
    {
        wait(&res);
    }

    // clean semaphores
    for (i = 0; i < N_PHILOSOPHERS; i++)
    {
        sem_close(sem_ids[i]);
        sem_unlink(sem_paths[i]);
    }

    sem_close(state_mutex);
    sem_unlink(STATE_MUTEX_PATH);

    // clean shared memory
    munmap(shm_ptr, shared_seg_size);
    shm_unlink(SHM_PATH);

    return 0;
}