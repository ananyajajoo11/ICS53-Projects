#include <signal.h>
#include <stdbool.h>
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
  bool active;
} Job;

int pid = -1;
int job_id = 0;
int noOfJobs = 0;
char command[128] = "";
Job jobs[5];
int next_available_job_id = 1;

void prompt() { printf("prompt >"); }

void sigchldHandler(int signo) {
  int status;
  pid_t child_pid;
  while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
    // Child process terminated, handle as needed
    printf("Child process with PID %d has terminated\n", child_pid);

    for (int i = 0; i < 5; i++) {
      if (jobs[i].pid == child_pid) {
        jobs[i].active = false;
        printf("%d ", jobs[i].pid);
        // Remove the job by shifting the remaining jobs
        /*for (int j = i; j < noOfJobs - 1; j++) {
          jobs[j] = jobs[j + 1];
        }*/
        noOfJobs--;
        printf("Removed job with PID %d from the Job structure\n", child_pid);
        break;
      }
    }
  }
}

Job create_job(int pid, int state, char command_line[]) {
  Job new_job;
  new_job.job_id = 0;  // Assign a new job ID and increment last_job_id
  new_job.pid = pid;
  new_job.state = state;
  // new_job.command_line = command_line;
  strncpy(new_job.command_line, command_line, sizeof(new_job.command_line));
  new_job.active = false;
  return new_job;
}

void add_job(Job new_job) {
  printf("no of jobs %d ", noOfJobs);
  if (noOfJobs < 5) {
    int new_job_id = 1;

    // Find the lowest available job ID
    /*for (int i = 0; i < noOfJobs; i++) {
      if (jobs[i].active && jobs[i].job_id == new_job_id) {
        new_job_id++;
        i = -1;  // Start over to ensure uniqueness
      }
    }*/
    // Add the new job to the first available spot
    for (int i = 0; i < 5; i++) {
      printf("Active? %d \n ", jobs[i].active);
      if (!jobs[i].active) {
        new_job.job_id = i + 1;
        jobs[i] = new_job;
        jobs[i].active = true;
        noOfJobs++;
        return;
      }
    }
  } else {
    printf("Job list is full. Cannot add more jobs.\n");
    return;
    // exit(0);
  }
}

void quitProg() {
  if (pid == 0) {
    printf("%d\n", pid);
    printf("Trying to exit child process\n");
    kill(getpid(), SIGINT);
  }
}

/*void stopForegroundJob() {
  if (pid == 0) {
    // printf("Coming the the kill with pid %d ", pid);
    // fflush(stdout);
    pid_t my_pid = getpid();
    printf("Coming to the kill with pid %d\n", my_pid);
    fflush(stdout);
    kill(pid, SIGTSTP);
    jobs[noOfJobs] = create_job(my_pid, 3, command);
    noOfJobs += 1;
  } else {
    printf("Coming the the exit with pid %d ", pid);
    fflush(stdout);
    // exit(0);
  }
  if (pid == 0) {
    // Send SIGTSTP to the child process (foreground job)
    kill(0, SIGTSTP);
  }
}*/

void stopForegroundJob() {
  if (pid == 0) {
    return;  // Do nothing for the child process
  }

  // For the parent process
  if (kill(pid, SIGTSTP) == 0) {
    // Successfully sent SIGTSTP to the child
    // jobs[noOfJobs] = create_job(pid, 3, command);
    // noOfJobs++;
    add_job(create_job(pid, 3, command));
  } else {
    perror("kill");
  }
}

void runninginforeground(char command[], char args[]) {
  pid = fork();
  if (pid == 0) {
    // pid = getpid();
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    //  signal(SIGINT, quitProg);
    sleep(30);
    char direc[1024];
    getcwd(direc, sizeof(direc));
    strcat(direc, "/");
    strcat(direc, command);
    // system(direc);
    if (execv(direc, args) < 0) {
      printf("Program not found");
      exit(0);
    }
  } else {
    signal(SIGINT, quitProg);
    signal(SIGTSTP, stopForegroundJob);
    int status;
    waitpid(pid, &status, WUNTRACED);
    // wait(NULL);
  }
}

void runninginbackground(char command[], char args[]) {
  int pid = fork();
  char amper[1024] = "";
  strcat(amper, command);
  strcat(amper, " &");
  add_job(create_job(pid, BACKGROUND_RUNNING, amper));
  // noOfJobs += 1;
  if (pid == 0) {
    signal(SIGINT, SIG_IGN);
    sleep(5);
    char direc[1024];
    getcwd(direc, sizeof(direc));
    strcat(direc, "/");
    strcat(direc, command);
    // system(direc);
    char* const argv[] = {direc, NULL};
    if (execv(direc, argv) < 0) {
      printf("Program not found");
      exit(0);
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
  for (int i = 0; i < noOfJobs; i++) {
    if (job_list[i].active) {
      printf("[%d] (%d) %s %s \n", job_list[i].job_id, job_list[i].pid,
             get_status_string(job_list[i].state), job_list[i].command_line);
    }
  }
}
// Function to get the status string based on the state

// Function to bring a job to the foreground
void bring_to_foreground(char* arg) {
  if (arg[0] == '%') {
    int job_id;
    bool found = 0;
    if (sscanf(arg, "%%%d", &job_id) == 1) {
      bool found = 0;
      for (int i = 0; i < noOfJobs; i++) {
        if (jobs[i].job_id == job_id) {
          kill(jobs[i].pid, SIGCONT);
          jobs[i].state = 1;
          found = 1;
          waitpid(jobs[i].pid, NULL, 0);
        }
      }
      if (!found) {
        printf("Job with ID %d not found\n", job_id);
      }

    } else {
      printf("Invalid job reference format\n");
    }
  } else {
    pid_t pid;
    if (sscanf(arg, "%d", &pid) == 1) {
      bool found = 0;
      int pid3 = atoi(arg);
      printf("%d \n", pid3);
      fflush(stdout);
      kill(pid3, SIGCONT);
      for (int i = 0; i < noOfJobs; i++) {
        if (jobs[i].pid == pid3) {
          found = 1;
          jobs[i].state = 1;
          waitpid(jobs[i].pid, NULL, 0);
        }
      }
      if (!found) {
        printf("Job with PID %d not found\n", pid);
      }

    } else {
      printf("Invalid job reference format\n");
    }
  }
}

void bring_to_background(const char* arg) {
  int job_ref;
  pid_t pid;

  if (arg[0] == '%') {
    if (sscanf(arg, "%%%d", &job_ref) == 1) {
      // Handle JID case
      for (int i = 0; i < noOfJobs; i++) {
        if (jobs[i].job_id == job_ref) {
          if (jobs[i].state == STOPPED) {
            // If the job is in the STOPPED state, send a SIGCONT signal to
            // resume it
            if (kill(jobs[i].pid, SIGCONT) == 0) {
              jobs[i].state = BACKGROUND_RUNNING;
              printf("Resumed job %d to Background/Running state\n", job_ref);
            } else {
              perror("kill");
            }
          } else if (jobs[i].state == BACKGROUND_RUNNING) {
            printf("Job %d is already in the Background/Running state\n",
                   job_ref);
          } else {
            printf("Job %d is in an invalid state\n", job_ref);
          }
          return;
        }
      }
      printf("Job with ID %d not found\n", job_ref);
    } else {
      printf("Invalid job reference format\n");
    }
  } else if (sscanf(arg, "%d", &pid) == 1) {
    // Handle PID case
    for (int i = 0; i < noOfJobs; i++) {
      if (jobs[i].pid == pid) {
        if (jobs[i].state == STOPPED) {
          // If the job is in the STOPPED state, send a SIGCONT signal to resume
          // it
          if (kill(jobs[i].pid, SIGCONT) == 0) {
            jobs[i].state = BACKGROUND_RUNNING;
            printf("Resumed job with PID %d to Background/Running state\n",
                   pid);
          } else {
            perror("kill");
          }
        } else if (jobs[i].state == BACKGROUND_RUNNING) {
          printf("Job with PID %d is already in the Background/Running state\n",
                 pid);
        } else {
          printf("Job with PID %d is in an invalid state\n", pid);
        }
        return;
      }
    }
    printf("Job with PID %d not found\n", pid);
  } else {
    printf("Invalid job reference format\n");
  }
}

void terminateJob(const char* arg) {
  int job_ref;
  pid_t pid;
  if (arg[0] == '%') {
    if (sscanf(arg, "%%%d", &job_ref) == 1) {
      // Handle JID case
      for (int i = 0; i < noOfJobs; i++) {
        if (jobs[i].job_id == job_ref) {
          pid = jobs[i].pid;
          if (kill(pid, SIGTERM) == 0) {
            jobs[i].active = false;
            // Successfully sent SIGINT to the job
            printf("Terminated job %d\n", job_ref);
          } else {
            perror("kill");
          }
          return;
        }
      }
      printf("Job with ID %d not found\n", job_ref);
    } else {
      printf("Invalid job reference format\n");
    }
  } else if (sscanf(arg, "%d", &pid) == 1) {
    // Handle PID case
    for (int i = 0; i < noOfJobs; i++) {
      if (jobs[i].pid == pid) {
        if (kill(pid, SIGTERM) == 0) {
          // Successfully sent SIGINT to the job
          printf("Terminated job with PID %d\n", pid);
          jobs[i].active = false;
        } else {
          perror("kill");
        }
        return;
      }
    }
    printf("Job with PID %d not found\n", pid);
  } else {
    printf("Invalid job reference format\n");
  }
}

int main() {
  signal(SIGCHLD, sigchldHandler);
  while (1) {
    char inp[1024];
    char cwd[1024];
    int command_count;
    prompt();
    fgets(inp, 128, stdin);
    char args[128] = "";
    command_count = sscanf(inp, "%s %s", command, args);
    // printf("%s %s\n", command, args);
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
    } else if (strcmp(command, "fg") == 0) {
      bring_to_foreground(args);
    } else if (strcmp(command, "bg") == 0) {
      // Handle "bg" command
      bring_to_background(args);
    } else if (strcmp(command, "") == 0) {
      printf("Empty Command");
    } else if (strcmp(command, "kill") == 0) {
      terminateJob(args);
    } else {
      if (strcmp(args, "") == 0) {
        runninginforeground(command, args);
      } else {
        runninginbackground(command, args);
      }
    }
  }
}