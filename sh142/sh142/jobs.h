//
//  jobs.h
//  sh142
//
//  Created by Øyvind Tangen on 17.11.11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include "definitions.h"

#ifndef sh142_jobs_h
#define sh142_jobs_h

#define FOREGROUND 'F'
#define BACKGROUND 'B'
#define SUSPENDED 'S'
#define WAITINGINPUT 'W'

#define STDIN 1
#define STDOUT 2

#define PROCESSID 1
#define JOBID 2
#define JOBSTATUS 3



typedef struct job {
    int id;             // Job ID
    char *name;         // 
    pid_t pid;          // Process ID
    pid_t pgid;         // Process group ID
    int status;         // Status value
    //char *descriptor;   // 
    struct job *next;   // Next active job
} job;

static int numberOfActiveJobs = 0;
static job* jobList = NULL;

static pid_t SHELL_PID;
static pid_t SHELL_PGID;
static int SHELL_TERMINAL;
static int SHELL_IS_INTERACTIVE;

//struct termios SHELL_TMODES;

void errormsg(char* c);
void childSignalHandler(int i);
void jobInit();
int launchJob(char* cmd[]);
void listJobs();

job* addJob(pid_t pid, pid_t pgid, char* jobName, int status);
int setJobStatus(int pid, int newStatus);
job* deleteJob(job *job);
job* getJob(int value, int type);

void setJobInBackground(job* j,/* int cont,*/ bool bg);
void waitJob(job* j);

#endif
