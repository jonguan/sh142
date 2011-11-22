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

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define STDINOUT 3

#define PROCESSID 1
#define JOBID 2


typedef struct job {
    int id;             // Job ID
    char *name;         // Job name
    pid_t pid;          // Process ID
    pid_t pgid;         // Process group ID
    long lastChecked;        //Time since last check by scheduler
    time_t timeOverCpuLimit;//Time process spent preceding CPU usage limit
    time_t timeOverMemLimit;//Time process spent preceding memory usage limit
    time_t timeAlive;       //Time the job has been alive in total
    int status;         // Status value
    char *descriptor;   // Descriptor
    struct job *next;   // Next active job
} job;

static int numberOfActiveJobs = 0;
//static job* jobList = NULL;

static pid_t SHELL_PID;
static pid_t SHELL_PGID;
static int SHELL_TERMINAL;
static int SHELL_IS_INTERACTIVE;
static struct termios SHELL_TMODES;

void shellInit(void);
int launchJob(char* cmd[], char* path, int flag, int mode);
job* addJob(pid_t pid, pid_t pgid, char* jobName,char* descriptor, int status);
void childSignalHandler(int i);
void setJobInBackground(job* j, int cont, bool bg);
//void setJobInBackground(job* j, int cont, bool bg, char* path, int rw);
void errormsg(char* c);
int setJobStatus(int pid, int newStatus);
job* deleteJob(job *job);
job* getJob(int value, int type);
void waitForJob(job* j);
job* getJobList(void);
void killJob(int id);
void listJobs(void);
void putToBackground(job* j);

int launchBackgroundJob(char* cmd[], char* path, int flag);

#endif
