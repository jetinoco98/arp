#include "wrapfunc.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int m_read(int fd, void *buf, size_t nbytes) {
    int ret = read(fd, buf, nbytes);
    if (ret < 0) {
        perror("Error on reading from file descriptor");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_fork() {
    int ret = fork();
    if (ret < 0) {
        perror("Error on forking");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}
int m_write(int fd, void *buf, size_t nbytes) {
    int ret = write(fd, buf, nbytes);
    if (ret < 0) {
        perror("Error on writing in file descriptor");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
             struct timeval *timeout) {
    int ret = select(nfds, readfds, writefds, exceptfds, timeout);
    if (ret < 0) {
        perror("Error on executing select");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_open(const char *file, int oflag) {
    int ret = open(file, oflag);
    if (ret < 0) {
        perror("Error on executing open");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_pipe(int *pipedes) {
    int ret = pipe(pipedes);
    if (ret < 0) {
        perror("Error on executing open");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_close(int fd) {
    int ret = close(fd);
    if (ret < 0) {
        perror("Error on executing open");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_shm_open(const char *name, int flag, mode_t mode) {
    int ret = shm_open(name, flag, mode);
    if (ret < 0) {
        perror("Error on executing shm_open");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_ftruncate(int fd, __off_t length) {
    int ret = ftruncate(fd, length);
    if (ret < 0) {
        perror("Error on executing ftruncate");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

void *m_mmap(void *addr, size_t len, int prot, int flags, int fd,
             off_t offset) {
    void *ret = mmap(addr, len, prot, flags, fd, offset);
    if (ret == MAP_FAILED) {
        perror("Error on executing mmap");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}
sem_t *m_sem_open(const char *name, int oflag, mode_t mode,
                  unsigned int value) {
    sem_t *ret = sem_open(name, oflag, mode, value);
    if (ret == SEM_FAILED) {
        perror("Error on executing sem_open");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_sem_init(sem_t *sem, int pshared, int value) {
    int ret = sem_init(sem, pshared, value);
    if (ret < 0) {
        perror("Error on executing sem_init");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_sem_wait(sem_t *sem) {
    int ret = sem_wait(sem);
    if (ret < 0) {
        perror("Error on executing sem_wait");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_sem_post(sem_t *sem){
    int ret = sem_post(sem);
    if (ret < 0) {
        perror("Error on executing sem_post");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_sem_close(sem_t *sem){
    int ret = sem_close(sem);
    if (ret < 0) {
        perror("Error on executing sem_close");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}

int m_sem_unlink(const char *name){
    int ret = sem_unlink(name);
    if (ret < 0) {
        perror("Error on executing sem_unlink");
        fflush(stdout);
        getchar();
        exit(EXIT_FAILURE);
    }
    return ret;
}
