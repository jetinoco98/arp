#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void spawn(char** programArgs){
    execvp("konsole",programArgs);
    perror("Execution failed");

    //avoid unwanted forking
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    char* argsFirst[] = {"konsole", "-e", "./first", NULL};
    char* argsSecond1[] = {"konsole", "-e", "./second", NULL};
    char* argsSecond2[] = {};

    for (int i = 0; i < 2; i++) {
        // Spawning processes as childs of the caller
        pid_t pid = fork();

        if(pid < 0){
            exit(EXIT_FAILURE);
        }else if(!pid){
            if(i == 0){
                spawn(argsFirst);
            }else if(i == 1){
                spawn(argsSecond1);
            }
        }else{
            // Continuing with father
            printf("Spawned child with pid: %d\n", pid);
        }
    }
    return EXIT_SUCCESS;
}
