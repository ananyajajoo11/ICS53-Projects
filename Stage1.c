#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 100

void print_prompt() {
  printf("prompt > ");
  // fflush(stdout);
}

int main() {
  char input[MAX_INPUT_SIZE];
  char cwd[1024];

  while (1) {
    print_prompt();

    if (fgets(input, sizeof(input), stdin) == NULL) {
      break;  // End of input (e.g., Ctrl+D)
    }

    // Remove the trailing newline character
    input[strcspn(input, "\n")] = '\0';

    char* command = strtok(input, " ");

    if (command == NULL) {
      continue;  // No command entered
    }

    if (strcmp(command, "cd") == 0) {
      char* directory = strtok(NULL, " ");
      if (directory == NULL) {
        fprintf(stderr, "Usage: cd <directory>\n");
      } else {
        if (chdir(directory) != 0) {
          perror("cd");
        }
      }
    } else if (strcmp(command, "pwd") == 0) {
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
      } else {
        perror("getcwd");
      }
    } else if (strcmp(command, "quit") == 0) {
      break;  // Exit the shell
    } else {
      printf("Unknown command: %s\n", command);
    }
  }

  return 0;
}
