#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd1[2];  // Used to store two ends of first pipe
    int fd2[2];  // Used to store two ends of second pipe

    char fixed_str1[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str1[100];
    char input_str2[100];
    pid_t p;

    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    printf("Enter first string to concatenate: ");
    scanf("%s", input_str1);
    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed");
        return 1;
    }

    // Parent process
    else if (p > 0)
    {
        close(fd1[0]);  // Close reading end of first pipe

        // Write input string to child
        write(fd1[1], input_str1, strlen(input_str1) + 1);
        close(fd1[1]);

        // Wait for child to send a string
        wait(NULL);

        // Read concatenated string from child
        read(fd2[0], input_str2, 100);
        close(fd2[0]);

        // Concatenate second fixed string
        strcat(input_str2, fixed_str2);
        printf("Final Output: %s\n", input_str2);
    }

    // Child process
    else
    {
        close(fd1[1]);  // Close writing end of first pipe
        
        // Read string from parent
        char concat_str[100];
        read(fd1[0], concat_str, 100);
        close(fd1[0]);

        // Concatenate first fixed string
        strcat(concat_str, fixed_str1);
        printf("First Output: %s\n", concat_str);

        printf("Enter second string to concatenate: ");
        scanf("%s", input_str2);

        // Write second input string to parent
        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]);

        exit(0);
    }

    return 0;
}
