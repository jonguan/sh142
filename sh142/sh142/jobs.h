//
//  jobs.h
//  sh142
//
//  Created by Øyvind Tangen on 17.11.11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#ifndef sh142_jobs_h
#define sh142_jobs_h

#include "definitions.h"

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
    char *descriptor;   // Descriptor (hehe)
    struct job *next;   // Next active job
} job;

static int numberOfActiveJobs = 0;
static job* jobList = NULL;

static pid_t SHELL_PID;
static pid_t SHELL_PGID;
static int SHELL_TERMINAL;
static int SHELL_IS_INTERACTIVE;
static struct termios SHELL_TMODES;

void newShellInit(void);

//int launchJob(char* cmd[], int mode, char* path, int flag);

int launchJob(char* cmd[], char* path, int flag, int mode);

void errormsg(char* c);
    
void childSignalHandler(int i);
void shellInit();
void listJobs();

job* addJob(pid_t pid, pid_t pgid, char* jobName,char* descriptor, int status);
int setJobStatus(int pid, int newStatus);
job* deleteJob(job *job);
job* getJob(int value, int type);

void setJobInBackground(job* j, int cont, bool bg);
void waitJob(job* j);
void killJob(int id);

void signalHandler_child(int p);
#endif
