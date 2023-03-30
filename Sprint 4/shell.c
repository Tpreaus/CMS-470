/**
*  Theodore Preaus & Philip Clarke CMS-470
*
*  Implement a custom shell in C using proceses methods such as fork 
*  and ecexvp
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(void) {

  // Variables
  int running = 0;

  // Main loop 
  while (running == 0) {
  
    // Display special prompt
    printf("My Shell >");
  
    // Get line of input from prompt
    char *buffer;
    // Create special variable for getline()
    size_t bufsize = 32;

    char* output_file;
    int stdout_copy;

    
    // Allocate buffer on the heap
    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }
    bufsize = getline(&buffer,&bufsize,stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    // Check if '>' is present in input line
    char *redirect_char = strstr(buffer, ">");
    // redirect_char is either a pointer to > or null
    if (redirect_char != NULL) {
      // Replace '>' with null char
      *redirect_char = '\0';

      output_file = redirect_char + 1;

      // get rid of leading white Space
      while (*output_file == ' ') {
        output_file++;
      }
      stdout_copy = dup(1);
      close(1);
      // change output
      open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    }
    // Parse the input line to get program + options
    char *token = strtok(buffer, " ");

    if (token) {
      // Check for special functions
      
      // If exit is used end shell
      if (strcmp(token, "exit") == 0) {
        return 0;
      }

      // If pwd is typed print present working directory
      if (strcmp(token, "pwd") == 0) {
        char *cwd = NULL;

        cwd = getcwd(NULL, 0);

        if (cwd != NULL) {
          printf("Current working directory: %s\n", cwd);
          free(cwd);
        } else {
          perror("getcwd() error");
          exit(EXIT_FAILURE);
        }
      }

      // If cd is input change the directory to input string
      if (strcmp(token, "cd") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL) {
          // No argument provided to cd, print error message
          printf("Error: no argument provided to cd\n");
        } else {
          // Call chdir to change directory
          if (chdir(token) != 0) {
            printf("Error: unable to change directory to %s\n", token);
          }
        }
      }
      
      
      // Call fork to make child process

      int rc = fork();
      // Rc from fork is 0 meaning its the child
   
      // Check to make sure its child
      if (rc == 0) {
        char *args[5];
        for (int i = 0; i < 5; i++) {
          args[i] = token;
          token = strtok(NULL, " ");
        }
        
        // Use execvp to run command prompts 
        execvp(args[0], args);

        // If command is invalid give error and exit child
        perror("execvp");
        return -1;
        
      }
      else if (rc > 0) {
        
        // The basic wait system call 
        // passing NULL as argument suspends the calling process 
        // until one of its children terminates
        int rc = wait(NULL);
        if (rc < 0) {
            perror("wait");
            return -1;
        }
        if (redirect_char != NULL) {
          dup2(stdout_copy, 1);
          close(stdout_copy);
          redirect_char = NULL;
        }
    }
    token = strtok(NULL, " ");
    }
  }  
  return 0;
}
