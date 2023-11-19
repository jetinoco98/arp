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
    char * myfifo1 = "/tmp/myfifo1";
    mkfifo(myfifo1, 0666);
    // FIFO for Children 2
    int fd2;
    char * myfifo2 = "/tmp/myfifo2";
    mkfifo(myfifo2, 0666);

    // VARIABLES DECLARATIONS
    char send_char[80];

    // Information about child process number
    int id = argv[1][0] - '0';
    printf(LOG_INFO "I am child number %d\n\n", id);

    while(1) {
        sleep(1);
        if (id == 1) {
            send_char[0] = 'A';
            fd1 = open(myfifo1, O_WRONLY);
            write(fd1, send_char, strlen(send_char)+1);
            close(fd1);
            usleep(100000);
        }
        
        else if (id == 2) {
            send_char[0] = 'B';
            fd2 = open(myfifo2, O_WRONLY);
            write(fd2, send_char, strlen(send_char)+1);
            close(fd2);
            usleep(500000);
        }
    }

    return 0;
}