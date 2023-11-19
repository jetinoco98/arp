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

    int fd1;
    char *myfifo1 = "./myfifo1";
    mkfifo(myfifo1, 0666);

    //char str1[80], str2[80];
    char format_string[80] = "%d,%d";
    int r1, r2;
    double f1, f2;

    // Information about child process number
    int id = argv[1][0] - '0';
    printf(LOG_INFO "Im am child number %d\n\n", id);
    fflush(stdout);

    sleep(5);

    return 0;
}