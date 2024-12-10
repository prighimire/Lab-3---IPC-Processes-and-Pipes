#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

/**
 * Executes the command "cat scores | grep <argument> | sort".
 * This creates three processes:
 * - P1 (Parent): executes "cat scores"
 * - P2 (Child): executes "grep <argument>"
 * - P3 (Child's Child): executes "sort"
 */

int main(int argc, char **argv)
{
    int pipefd1[2];  // Pipe between cat and grep
    int pipefd2[2];  // Pipe between grep and sort
    int pid1, pid2;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <search term>\n", argv[0]);
        exit(1);
    }

    char *cat_args[] = {"cat", "scores", NULL};  // cat command arguments
    char *grep_args[] = {"grep", argv[1], NULL}; // grep command with user argument
    char *sort_args[] = {"sort", NULL};          // sort command

    // Create first pipe
    if (pipe(pipefd1) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid1 = fork();

    if (pid1 < 0)
    {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0)
    {
        // Child process (P2): will handle "grep <argument>"

        // Create second pipe for grep to sort communication
        if (pipe(pipefd2) == -1)
        {
            perror("pipe");
            exit(1);
        }

        pid2 = fork();

        if (pid2 < 0)
        {
            perror("fork");
            exit(1);
        }

        if (pid2 == 0)
        {
            // Child's Child (P3): will handle "sort"

            // Replace standard input with input part of pipe 2 (output of grep)
            dup2(pipefd2[0], 0);

            // Close both ends of both pipes
            close(pipefd2[1]); // Close write end of pipe 2
            close(pipefd1[0]); // Close read end of pipe 1
            close(pipefd1[1]); // Close write end of pipe 1

            // Execute sort
            execvp("sort", sort_args);
            perror("execvp sort"); // Only reached if execvp fails
        }
        else
        {
            // Child process (P2): "grep <argument>"

            // Replace standard input with input part of pipe 1 (output of cat)
            dup2(pipefd1[0], 0);

            // Replace standard output with output part of pipe 2 (input to sort)
            dup2(pipefd2[1], 1);

            // Close all ends of both pipes
            close(pipefd1[1]); // Close write end of pipe 1
            close(pipefd2[0]); // Close read end of pipe 2
            close(pipefd2[1]); // Close write end of pipe 2

            // Execute grep
            execvp("grep", grep_args);
            perror("execvp grep"); // Only reached if execvp fails
        }
    }
    else
    {
        // Parent process (P1): will handle "cat scores"

        // Replace standard output with write end of pipe 1 (input to grep)
        dup2(pipefd1[1], 1);

        // Close both ends of both pipes
        close(pipefd1[0]); // Close read end of pipe 1
        close(pipefd1[1]); // Close write end of pipe 1

        // Execute cat
        execvp("cat", cat_args);
        perror("execvp cat"); // Only reached if execvp fails
    }

    return 0;
}
