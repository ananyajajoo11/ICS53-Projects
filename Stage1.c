#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void prompt() { printf("\nprompt >"); }

void quitProg() {
  // int pid = getpid();
  // printf("%d\n", pid);
  //  printf("Coming here");
  /*if (pid == 0) {
    printf("Trying to exit child process");
    exit(0);
    // kill(pid, SIGINT);
  }*/
  // exit(0);
}

void runningfile(char command[],char args[]) {
  int pid = fork();
  if (pid == 0) {
    sleep(3);
    char direc[1024];
    getcwd(direc, sizeof(direc));
    strcat(direc, "/");
    strcat(direc, command);
    //system(direc);
    if(execv(direc,args)<0){
      printf("Program not found");
      exit(0);
    }
    //printf("\n");
    exit(0);
  } else {
    signal(SIGINT, quitProg);
    wait(NULL);
  }
}

int main() {
  while (1) {
    char inp[1024];
    char cwd[1024];
    int command_count;
    prompt();
    fgets(inp, 128, stdin);
    char command[128]="", args[128]="";
    command_count = sscanf(inp, "%s %s", command, args);
    // printf("%s %s\n", command, args);
    if (strcmp(command, "pwd") == 0) {
      // char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s", cwd);
      } else {
        perror("getcwd() error");
      }
    }

    else if (strcmp(command, "cd") == 0) {
      if (chdir(args) != 0) {
        perror("getcwd");
      }
    }

    else if (strcmp(command, "quit") == 0) {
      break;
    } 
    else if(strcmp(command,"")==0){
      printf("Empty Command");
      }
      else {
      runningfile(command,args);
      /*int pid = fork();
      if (pid == 0) {
        runningfile(command);
        break;
      } else {
        wait(NULL);
        exit()
      }*/
    }

    /*else {
      printf("Unknown command\n");
    }*/
  }
}