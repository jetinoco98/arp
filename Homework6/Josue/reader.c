#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 1024

int main(int argc, char *argv[]) {
    // Create FIFOs
    char *myfifo1 = "/tmp/myfifo1";
    char *myfifo2 = "/tmp/myfifo2";
    mkfifo(myfifo1, 0666);
    mkfifo(myfifo2, 0666);

    // Open FIFOs for reading
    int fd1 = open(myfifo1, O_RDONLY | O_NONBLOCK);
    int fd2 = open(myfifo2, O_RDONLY | O_NONBLOCK);

    // Open the log file for writing
    FILE *logFile = fopen("log.txt", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd1, &rfds);
        FD_SET(fd2, &rfds);

        // Use select to check for data on either fd1 or fd2

        // Select needs as first argument the file descriptor whith the highest value, and +1
        // 
        
        if (select(fd2 + 1, &rfds, NULL, NULL, NULL) == -1) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        char buffer[MAX_BUF];
        int bytesRead;

        // Check fd1
        if (FD_ISSET(fd1, &rfds)) {
            bytesRead = read(fd1, buffer, MAX_BUF);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                fprintf(logFile, "%s", buffer);
                fflush(logFile);
            }
        }

        // Check fd2
        if (FD_ISSET(fd2, &rfds)) {
            bytesRead = read(fd2, buffer, MAX_BUF);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                fprintf(logFile, "%s", buffer);
                fflush(logFile);
            }
        }
    }

    // Cleanup: Close file descriptors and file
    close(fd1);
    close(fd2);
    fclose(logFile);

    return 0;
}


