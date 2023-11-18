#include <semaphore.h>
#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/mman.h>
#include <unistd.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include "include/constants.h"

sem_t * mutex;
sem_t * S[N_PHILOSOPHERS]; // Semaphores for each philospher (not for each fork)
int state[N_PHILOSOPHERS];
int shared_seg_size;
int philosopher_num;

void *shm_ptr;

FILE * fp;


// wrapper for opening the file, writing the line and then closing the file
void ph_log(char * format, int phnum)
{
    fp = fopen(LOG_FILE, "a");
    fprintf(fp, format, phnum);
    fclose(fp);
}

// Not always testing self, will test neighbors after putting down forks to signal them that self is no longer using the forks
void test(int phnum)
{
    if (state[phnum] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {
        // state that eating
        state[phnum] = EATING;
        // Allows take fork to continue either for self or someone waiting on fork just put down
        sem_post(S[phnum]);
    } 

}
 
// take up chopsticks
void take_fork(int phnum)
{
 
    sem_wait(mutex);

    memcpy(state, shm_ptr, shared_seg_size);
    
    // state that hungry
    state[phnum] = HUNGRY;
 
    printf("Philosopher %d is Hungry\n", phnum + 1);
    ph_log("Philospher %d is Hungry\n", phnum + 1);
    // if able to eat, post self semaphore and set self state to eating
    test(phnum);

    memcpy(shm_ptr, state, shared_seg_size);

    sem_post(mutex);
 
    // if unable to eat wait to be signalled
    sem_wait(S[phnum]);

    printf("Philosopher %d takes fork %d and %d\n",
                        phnum + 1, LEFT + 1, phnum + 1);
    printf("Philosopher %d is Eating\n", phnum + 1);
    // log to file
    ph_log("Philosopher %d is Eating \n", phnum + 1);

}
 
// put down chopsticks, tests left and right (which will post the semaphores for them if they can go)
void put_fork(int phnum)
{
    // wrap with mutex and memcopy for accessing the shared state
    sem_wait(mutex);

    memcpy(state, shm_ptr, shared_seg_size);
 
    // state that thinking
    state[phnum] = THINKING;
 
    printf("Philosopher %d putting fork %d and %d down\n",
           phnum + 1, LEFT + 1, phnum + 1);
    printf("Philosopher %d is thinking\n", phnum + 1);
    // log to file
    ph_log("Philosopher %d putting down forks\n", phnum + 1);

    // Post semaphores and set to eating for left and right if they can eat
    test(LEFT);
    test(RIGHT);

    memcpy(shm_ptr, state, shared_seg_size);
 
    sem_post(mutex);
}



int main(int argc, char *argv[]) 
{

    // extract philosopher number
    if(argc == 2){
        sscanf(argv[1],"%i", &philosopher_num);
    } else {
        printf("wrong number of arguments\n"); 
        return -1;
    }

    char * sem_paths[N_PHILOSOPHERS];
    char tmp_path[80];

    // make semaphores for each philosopher (not for each fork)
    for (int i = 0; i < N_PHILOSOPHERS; i++)
    {
        sprintf(tmp_path, SEMAPHORE_PATH_FORMAT, i);
        sem_paths[i] = tmp_path;
        S[i] = sem_open(sem_paths[i], 0);
    }

    // Mutex for state shared memory
    mutex = sem_open(STATE_MUTEX_PATH, 0);

    // Open and map shared memory for state
    shared_seg_size = (1 * sizeof(state));
    int shmfd  = shm_open(SHM_PATH, O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0)
    {
        perror("shm_open");
        return -1;
    }
    shm_ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    printf("philospher number %d\n", philosopher_num);

    while (1) 
    {   
        take_fork(philosopher_num);
        sleep(3); // eat for 3 seconds
        put_fork(philosopher_num);
        sleep(2);
    } 

    // clean up shared memory and semaphores
    shm_unlink(SHM_PATH);

    for (int i = 0; i < N_PHILOSOPHERS; i++)
    {
        sem_close(S[i]);
        sem_unlink(sem_paths[i]);
    }

    sem_close(mutex);
    sem_unlink(STATE_MUTEX_PATH);

    return 0; 
} 