#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void prompt() { printf("prompt >"); }

int main() {
  while (1) {
    char inp[1024];
    char cwd[1024];
    int command_count;
    prompt();
    fgets(inp, 128, stdin);
    char command[128], args[128];
    command_count = sscanf(inp, "%s %s", command, args);
    if (strcmp(command, "pwd") == 0) {
      // char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
      } else {
        perror("getcwd() error");
      }
    }

    if (strcmp(command, "cd") == 0) {
      if (chdir(args) != 0) {
        perror("getcwd");
      }
    }

    if (strcmp(command, "quit") == 0) {
      break;
    }

    else {
      printf("Unknown command");
    }
  }
}