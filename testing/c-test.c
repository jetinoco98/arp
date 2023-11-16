#include <stdio.h>
#include <string.h>

int main() 
{ 
    char send_string[80];
    char input_string[80];
    
    printf("Please, write '1' or '2' to select the output reader or q to quit\n");
    fflush(stdout);
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

    printf("%s",send_string);
    printf("\n");
    //printf("%s",input_string);
    //printf("\n");
    
    return 0; 
}