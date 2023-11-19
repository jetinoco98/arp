#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void summon(char** programArgs){
    execvp("konsole",programArgs);
    perror("Execution failed");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int childExitStatus;
    char* argsReader[] = {"konsole", "-e", "./reader", NULL};
    char* argsWriter[] = {"konsole", "-e", "./writer", "0", NULL};

    for (int i = 0; i < 3; i++) {
        // Summoning processes as childs of the caller
        pid_t pid = fork();

        if(pid < 0){
            exit(EXIT_FAILURE);
        }
        else if(!pid){
            if (i == 0){
                summon(argsReader);
            }
            else if(i == 1){
                argsWriter[3] = "1";
                summon(argsWriter);
            }
            else if (i == 2) {
                argsWriter[3] = "2";
                summon(argsWriter);
            }
        }
        else{
            // Continue with father code
            printf("Summoned child with pid: %d\n", pid);
            fflush(stdout);
        }
    }

    for (int i = 0; i < 3; i++) {
        //waits for childrens to end
        int finishedPid =  wait(&childExitStatus);
        //if the childs have exited normally print their pid and status
        if(WIFEXITED(childExitStatus)){
            printf("Child %d terminated with exit status: %d\n", finishedPid, WEXITSTATUS(childExitStatus));
        }
    }

    printf("Childs exited and FIFO closed, father closing\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}
