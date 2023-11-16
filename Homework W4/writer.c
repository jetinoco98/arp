#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int fd;

    char *myfifo = "./myfifo";
    mkfifo(myfifo, 0666);

    char input_string[80], send_string[80];

    while (1) {
        printf("Please, write '1' or '2' to select the output reader or q to quit\n");
        fflush(stdout);
        // Reader selection string
        fgets(send_string, 80, stdin);

        if (send_string[0] != 'q') {
            printf("Please, write two integer numbers separated by comma (,)\n");
            fflush(stdout);
            // Number string
            fgets(input_string, 80, stdin);

            // removing trailing newline
            send_string[strcspn(send_string, "\r\n")] = 0;
            // formatting the input
            strcat(send_string, "|");
            strcat(send_string, input_string);
            // removing trailing newline
            send_string[strcspn(send_string, "\r\n")] = 0;
        }

        // sending formatted input
        fd = open(myfifo, O_WRONLY);
        write(fd, send_string, strlen(send_string) + 1);
        close(fd);

        /* if the first input char is q, exit  */
        if (send_string[0] == 'q') {
            fd = open(myfifo, O_WRONLY);
            write(fd, "q\n", 3);
            //cleaning
            close(fd);
            unlink(myfifo);

            exit(EXIT_SUCCESS);
        }
    }
    return 0;
}
