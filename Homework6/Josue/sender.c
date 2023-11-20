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

#define SLEEP_A 100000
#define SLEEP_B 500000

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
            // Error checking on fd1
            if (fd1 == -1) {
                perror(LOG_ERROR "Error opening FIFO for writing");
                exit(EXIT_FAILURE);
            }
            // Write function execution and error checking in same line
            if (write(fd1, send_char, strlen(send_char) + 1) == -1) {
                perror(LOG_ERROR "Error writing to FIFO");
            }
            close(fd1);
            usleep(SLEEP_A);
        }
        
        else if (id == 2) {
            send_char[0] = 'B';
            fd2 = open(myfifo2, O_WRONLY);
            if (fd2 == -1){
                perror(LOG_ERROR "Error opening FIFO for writing");
                exit(EXIT_FAILURE);
            }
            if (write(fd2, send_char, strlen(send_char)+1) == -1){
                perror(LOG_ERROR "Error writing to FIFO");
            };
            close(fd2);
            usleep(SLEEP_B);
        }
    }

    return 0;
}