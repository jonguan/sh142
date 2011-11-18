//
//  jobs.h
//  sh142
//
//  Created by Øyvind Tangen on 17.11.11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#ifndef sh142_jobs_h
#define sh142_jobs_h

typedef struct job {
    int id;             // Job ID
    char *name;         // 
    pid_t pid;          // Process ID
    pid_t pgid;         // Process group ID
    int status;         // Status value
    char *descriptor;   // 
    struct job *next;   // Next active job
} job;

//static int numActiveJobs = 0;
//static job* jobList = NULL;

void putJobInForeground(job* job, int cont);
void putJobInBackground(job* job, int cont);
void waitJob(job* job);
void killJob(int jobId);
job* delJob(job* job);

#endif
