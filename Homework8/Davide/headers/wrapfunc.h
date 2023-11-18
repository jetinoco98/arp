#ifndef WRAPFUNC
#define WRAPFUNC
#include <semaphore.h>
#include <stddef.h>
#include <sys/select.h>

int m_read(int fd, void *buf, size_t nbytes);
int m_write(int fd, void *buf, size_t nbytes);
int m_fork();
int m_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
             struct timeval *timeout);
int m_open(const char *file, int oflag);
int m_pipe(int *pipedes);
int m_close(int fd);
int m_shm_open(const char *name, int flag, __mode_t mode);
int m_ftruncate(int fd, __off_t length);
void *m_mmap(void *addr, size_t len, int prot, int flags, int fd,
             __off_t offset);
sem_t *m_sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
int m_sem_init(sem_t *sem, int pshared, int value);
int m_sem_wait(sem_t *sem);
int m_sem_post(sem_t *sem);
int m_sem_close(sem_t *sem);
int m_sem_unlink(const char *name);
#endif // !WRAPFUNC
