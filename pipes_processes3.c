#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  int pipefd1[2]; 
  int pipefd2[2];
  int pid1, pid2;

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL}; 
  char *sort_args[] = {"sort", NULL};

  // Create two pipes
  pipe(pipefd1); 
  pipe(pipefd2);

  pid1 = fork();
  if (pid1 == 0) {
    // Child process 1 (grep)
    dup2(pipefd1[0], STDIN_FILENO);  // Replace stdin with read end of pipe 1
    dup2(pipefd2[1], STDOUT_FILENO); // Replace stdout with write end of pipe 2
    close(pipefd1[1]);  // Close unused write end of pipe 1 
    close(pipefd2[0]);  // Close unused read end of pipe 2
    execvp("grep", grep_args); // Execute grep
  } 
  else {
    pid2 = fork();
    if (pid2 == 0) {
      // Child process 2 (sort)  
      dup2(pipefd2[0], STDIN_FILENO);  // Replace stdin with read end of pipe 2
      close(pipefd1[0]);  // Close unused read end of pipe 1
      close(pipefd1[1]);  // Close unused write end of pipe 1
      close(pipefd2[1]);  // Close unused write end of pipe 2
      execvp("sort", sort_args); // Execute sort
    }
    else {  
      // Parent process (cat)
      dup2(pipefd1[1], STDOUT_FILENO); // Replace stdout with write end of pipe 1
      close(pipefd1[0]);  // Close unused read end of pipe 1 
      close(pipefd2[0]);  // Close unused read end of pipe 2
      close(pipefd2[1]);  // Close unused write end of pipe 2
      execvp("cat", cat_args); // Execute cat
    }
  }
  return 0;
}