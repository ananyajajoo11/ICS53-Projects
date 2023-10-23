#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FOREGROUND_RUNNING 1
#define BACKGROUND_RUNNING 2
#define STOPPED 3

typedef struct {
  int job_id;
  pid_t pid;
  char command_line[256];  // Adjust the size as needed
  int state;               // You can use constants to represent states
} Job;

int pid = -1;
int job_id = 0;
int noOfJobs = 0;
Job jobs[30];

void prompt() { printf("prompt >"); }

void removejob(){
  noOfJobs--;
  
}

void sigchldHandler(int signo) {
  int status;
  pid_t child_pid;
  while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
    // Child process terminated, handle as needed
    printf("Child process with PID %d has terminated\n", child_pid);
  }
}

Job create_job(int pid, int state, char command_line[]) {
  Job new_job;
  new_job.job_id = ++job_id;  // Assign a new job ID and increment last_job_id
  new_job.pid = pid;
  new_job.state = state;
  // new_job.command_line = command_line;
  strncpy(new_job.command_line, command_line, sizeof(new_job.command_line));
  return new_job;
}

void quitProg() {
  // int pid = getpid();
  // printf("%d\n", pid);
  //  printf("Coming here");
  /*if (pid == 0) {
    printf("Trying to exit child process");
    exit(0);
    // kill(pid, SIGINT);
  }
  if (pid == 0) {
    printf("%d\n", pid);
    printf("Trying to kill child process");
    kill(pid, SIGINT);
  }*/

  if (pid == 0) {
    printf("%d\n", pid);
    printf("Trying to exit child process\n");
    kill(getpid(), SIGINT);
  }

  // exit(0);
}

void runninginforeground(char command[], char args[]) {
  //printf("Before checking%d", noOfJobs);
  pid = fork();
  printf("pid = %d ", pid);
  if (pid!=0){
    wait(NULL);
  }
  else{
    printf("123pid = %d ", pid);
    jobs[noOfJobs] = create_job(pid, 1, command);
    noOfJobs += 1;
    signal(SIGINT, SIG_DFL);
    sleep(3);
    char direc[1024];
    getcwd(direc, sizeof(direc));
    strcat(direc, "/");
    strcat(direc, command);
    if (execv(direc, args) < 0) {
      if(execvp(command,args)<0){
      //removejob();
      noOfJobs--;}
  }
  }
  
  // printf("pid = %d", pid);
  /*jobs[noOfJobs] = create_job(pid, 1, command);
  // printf("job id %d", job_id);
  noOfJobs += 1;
  // printf("%d\n", pid);
  if (pid == 0) {
    //jobs[noOfJobs] = create_job(pid, 1, command);
    //noOfJobs += 1;
    //sprintf("Before checking%d", noOfJobs);
    printf("Just checking child process");
    signal(SIGINT, SIG_DFL);
    // signal(SIGINT, quitProg);
    sleep(3);
    char direc[1024];
    getcwd(direc, sizeof(direc));
    strcat(direc, "/");
    strcat(direc, command);
    // system(direc);
    if (execv(direc, args) < 0) {
      if(execvp(command,args)<0){
      //removejob();
      noOfJobs--;
      printf("after checking %d", noOfJobs);
      printf("Program not found");
      exit(0);
    }
    }
  } else {
    signal(SIGINT, quitProg);
    wait(NULL);
  }*/
}

void runninginbackground(char command[], char args[]) {
  int pid = fork();
  char amper[1024]="";
  strcat(amper,command);
  strcat(amper," &");
  jobs[noOfJobs] = create_job(pid, 2, amper);
  noOfJobs += 1;
  // strcat(command, "&");

  if (pid == 0) {
    signal(SIGINT, SIG_IGN);
    sleep(5);
    char direc[1024];
    getcwd(direc, sizeof(direc));
    strcat(direc, "/");
    strcat(direc, command);
    // system(direc);
    char* const argv[] = {direc, NULL};
    if (execv(direc, args) < 0) {
      if(execvp(command,args)<0){
      printf("Program not found");
      exit(0);
    }
    }

    // printf("\n");
  } else {
    printf("Running %s in the background with PID: %d\n", command, pid);
  }
}

const char* get_status_string(int state) {
  switch (state) {
    case FOREGROUND_RUNNING:
      return "Running";
    case BACKGROUND_RUNNING:
      return "Running";
    case STOPPED:
      return "Stopped";
    default:
      return "Unknown";
  }
}

void print_job_list(Job job_list[]) {
  printf("while prijting %d", noOfJobs);
  for (int i = 0; i < noOfJobs; i++) {
    printf("[%d] (%d) %s %s \n", job_list[i].job_id, job_list[i].pid,
           get_status_string(job_list[i].state), job_list[i].command_line);
  }
}
// Function to get the status string based on the state

int main() {
  signal(SIGCHLD, sigchldHandler);
  while (1) {
    char inp[1024];
    char cwd[1024];
    int command_count;
    prompt();
    fgets(inp, 128, stdin);
    char command[128] = "", args[128] = "";
    command_count = sscanf(inp, "%s %s", command, args);
    // printf("%s %s\n", command, args);
    printf("%s c - ",command);
    printf("%s a - ",args);
    if (strcmp(command, "pwd") == 0) {
      // char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
      } else {
        perror("getcwd() error");
      }
    }

    else if (strcmp(command, "cd") == 0) {
      if (chdir(args) != 0) {
        perror("getcwd");
      }
    }

    else if (strcmp(command, "jobs") == 0) {
      print_job_list(jobs);
    }

    else if (strcmp(command, "quit") == 0) {
      break;
    } else if (strcmp(command, "") == 0) {
      printf("Empty Command");
    } else {
      if (strcmp(args, "") == 0) {
        //printf("Going to foreground");
        runninginforeground(command, args);
      } else {
        runninginbackground(command, args);
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