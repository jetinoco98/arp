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
        // Declares a file descriptor set named rfds. 
        // This set is used to specify the file descriptors that select should monitor.
        fd_set rfds;
        // Initializes the file descriptor set rfds by clearing all file descriptors from it.
        FD_ZERO(&rfds);
        // Adds fd1 and fd2 to the file descriptor set rfds.
        FD_SET(fd1, &rfds);
        FD_SET(fd2, &rfds);

        // USE OF SELECT FUNCTION
        // This function will check for data on the file descriptors.

        // Select needs as first argument the file descriptor whith the highest value, and +1.
        // We use fd2 because it was opened at the end, so its value should be the highest.
        if (select(fd2 + 1, &rfds, NULL, NULL, NULL) == -1) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        char buffer[MAX_BUF];
        int bytesRead;

        // CHECK FD1
        // If fd1 is set in rfds, it means that select has detected readability on fd1.
        if (FD_ISSET(fd1, &rfds)) {
            bytesRead = read(fd1, buffer, MAX_BUF);
            if (bytesRead > 0) {
                // This line null-terminates the portion of the buffer that contains the data read from fd1.
                buffer[bytesRead] = '\0';
                fprintf(logFile, "%s", buffer);
                fflush(logFile);
            }
        }

        // CHECK FD2
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


