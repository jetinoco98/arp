#ifndef CONSTANTS_H
#define CONSTANTS_H

#define N_PHILOSOPHERS 5

#define STATE_MUTEX_PATH "/sem_state_mutex"
#define SEMAPHORE_PATH_FORMAT "/sem_%d"
#define SHM_PATH "/ph_states"

#define THINKING 2
#define HUNGRY 1
#define EATING 0

#define LEFT (phnum + 4) % N_PHILOSOPHERS
#define RIGHT (phnum + 1) % N_PHILOSOPHERS

#define LOG_FILE "./log/philosophers.txt"

#endif // !CONSTANTS_H