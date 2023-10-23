#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void prompt() { printf("\nprompt >"); }

void sigchldHandler(int signo) {
  int status;
  pid_t child_pid;
  while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
    // Child process terminated, handle as needed
    printf("Child process with PID %d has terminated\n", child_pid);
  }
}

void quitprog() {
  // int pid = getpid();
  // printf("%d\n", pid);
  //  printf("Coming here");
  /*if (pid == 0) {
    printf("Trying to exit child process");
    exit(0);
    // kill(pid, SIGINT);
  }*/
  // exit(0);
  kill(getpid(),SIGINT);
}

void runninginforeground(char command[],char args[]) {
  int pid = fork();
  if (pid == 0) {
    // In the child process
    signal(SIGINT, SIG_DFL); // Use the default SIGINT handling in the child processes
    // Rest of the child process code
}
  if (pid == 0) {
    sleep(3);
    char direc[1024];
    getcwd(direc, sizeof(direc));
    strcat(direc, "/");
    strcat(direc, command);
    //system(direc);
    if(execv(direc,args)<0){
      perror("execv");
      exit(1);
    }
    //printf("\n");
    exit(0);
  } else {
    signal(SIGINT, quitprog);
    wait(NULL);
  }
}

void runninginbackground(char command[],char args[]){
  int pid = fork();
  /*if (pid == 0) {
    // In the child process
    signal(SIGINT, SIG_DFL); // Use the default SIGINT handling in the child processes
    // Rest of the child process code
}*/
  if (pid == 0) {
    sleep(25);
    char direc[1024];
    getcwd(direc, sizeof(direc));
    strcat(direc, "/");
    strcat(direc, command);
    //system(direc);
    char* const argv[]={direc,NULL};
    if(execv(direc,argv)<0){
      perror("execv");
      exit(1);
    }
    //printf("\n");
  } else {
    printf("Running %s in the background with PID: %d\n", command, pid);
  }

}
int main() {
  signal(SIGCHLD, sigchldHandler);
  signal (SIGINT,quitprog);
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
        if(strcmp(args,"")==0){

        
      runninginforeground(command,args);
        }
        else{
          runninginbackground(command,args);
        }
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