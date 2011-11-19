//
//  jobs.h
//  sh142
//
//  Created by Øyvind Tangen on 17.11.11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#ifndef sh142_jobs_h
#define sh142_jobs_h

#define FOREGROUND 'F'
#define BACKGROUND 'B'
#define SUSPENDED 'S'
#define WAITING_INPUT 'W'

#define STDIN 1
#define STDOUT 2

#define BY_PROCESS_ID 1
#define BY_JOB_ID 2
#define BY_JOB_STATUS 3



typedef struct job {
    int id;             // Job ID
    char *name;         // 
    pid_t pid;          // Process ID
    pid_t pgid;         // Process group ID
    int status;         // Status value
    char *descriptor;   // 
    struct job *next;   // Next active job
} job;

//static pid_t SHELL_PID;
//static pid_t SHELL_PGID;
//static int SHELL_TERMINAL;
//static int SHELL_IS_INTERACTIVE;
//struct termios SHELL_TMODES;

//static int numActiveJobs = 0;
static job* jobList = NULL;

int launchJob(char cmd[]);
#endif
