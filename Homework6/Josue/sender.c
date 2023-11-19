#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LOG_INFO "\x1b[32m[INFO] \x1b[0m"
#define LOG_ERROR "\x1b[31m[INFO] \x1b[0m"

int main(int argc, char *argv[]) {

    // FIFO for Children 1
    int fd1;
    char *myfifo1 = "./myfifo1";
    mkfifo(myfifo1, 0666);
    // FIFO for Children 2
    int fd2;
    char *myfifo2 = "./myfifo2";
    mkfifo(myfifo2, 0666);

    // VARIABLES DECLARATIONS
    char send_char[80];

    // Information about child process number
    int id = argv[1][0] - '0';
    printf(LOG_INFO "I am child number %d\n\n", id);

    int i = 0;
    while(i < 100) {
        printf("%d\n", id);
        if (id == 1) {
            printf("Inside id 1\n");
            send_char[0] = 'A';
            fd1 = open(myfifo1, O_WRONLY);
            write(fd1, &send_char, strlen(send_char)+1);
            close(fd1);
            printf("Sending character A to reader.\n");
            sleep(0.1);
        }
        else {
            printf("Inside id 2\n");
            send_char[0] = 'B';
            fd2 = open(myfifo2, O_WRONLY);
            write(fd2, &send_char, strlen(send_char)+1);
            close(fd2);
            sleep(0.9);
            printf("Sending character B to reader.\n");
        }
        i = i + 1;
    }

    return 0;
}