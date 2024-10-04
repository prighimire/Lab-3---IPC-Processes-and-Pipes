#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() 
{ 
    int fd1[2];  // First pipe for P1 -> P2 communication 
    int fd2[2];  // Second pipe for P2 -> P1 communication 

    char fixed_str1[] = "howard.edu";  // To concatenate in P2
    char fixed_str2[] = "gobison.org"; // To concatenate in P1
    char input_str[100]; 
    pid_t p; 

    // Create both pipes
    if (pipe(fd1) == -1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 

    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 

    // Fork a child process
    p = fork(); 

    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process (P1)
    else if (p > 0) 
    { 
        close(fd1[0]);  // Close reading end of pipe 1 (P1 -> P2)
        close(fd2[1]);  // Close writing end of pipe 2 (P2 -> P1)

        // Send the input string to P2
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]);  // Close writing end of pipe 1

        // Wait for the child process (P2) to finish
        wait(NULL); 

        // Read the second input from P2
        char second_input[100];
        read(fd2[0], second_input, 100); 

        // Concatenate "gobison.org" to the string received from P2
        int k = strlen(second_input);
        int i;
        for (i = 0; i < strlen(fixed_str2); i++) 
            second_input[k++] = fixed_str2[i]; 

        second_input[k] = '\0'; // Null terminate the string

        // Print the final concatenated string
        printf("Final concatenated string: %s\n", second_input);

        close(fd2[0]);  // Close reading end of pipe 2
    } 
  
    // Child process (P2)
    else
    { 
        close(fd1[1]);  // Close writing end of pipe 1 (P1 -> P2)
        close(fd2[0]);  // Close reading end of pipe 2 (P2 -> P1)

        // Read the input string from P1
        char concat_str[100]; 
        read(fd1[0], concat_str, 100); 
        close(fd1[0]);  // Close reading end of pipe 1

        // Concatenate "howard.edu" to the string
        int k = strlen(concat_str); 
        int i;
        for (i = 0; i < strlen(fixed_str1); i++) 
            concat_str[k++] = fixed_str1[i]; 

        concat_str[k] = '\0'; // Null terminate the string

        // Print the concatenated string
        printf("Concatenated string: %s\n", concat_str);

        // Prompt the user for another input
        char second_input[100];
        printf("Enter another string to concatenate: ");
        scanf("%s", second_input); 

        // Send the second input back to P1 via pipe 2
        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]);  // Close writing end of pipe 2

        exit(0); 
    } 
} 