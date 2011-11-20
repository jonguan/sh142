//
//  jobs.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 San Jose State University. All rights reserved.
//

//#include <iostream>
//#include <unistd.h>
#include "jobs.h"
//#include "main.h"

int launchJob(char* cmd[])
{
    return 0;
}

void errormsg(char* c) {
    printf("Error: %s\n", c);
}

job* addJob(pid_t pid, pid_t pgid, char* jobName, int status)
{
    job *j = (job*) malloc(sizeof(job));
    
    j->name = (char*) malloc(sizeof(jobName));
    j->name = strcpy(j->name, jobName);
    j->pid = pid;
    j->pgid = pgid;
    j->status = status;
    //j->descriptor = (char*) malloc(sizeof(descritor));
    //j->descriptor = strcpy(j->descriptor, descriptor);
    j->next = NULL;
    
    if (jobList == NULL) {
        numberOfActiveJobs++;
        j->id = numberOfActiveJobs;
        return j;
    }
    else {
        job *t = jobList;
        while (t->next != NULL) {
            t = t->next;
        }
        j->id = t->id+1;
        t->next = j;
        numberOfActiveJobs++;
        return jobList;
    }
}

void jobInit()
{
    SHELL_PID = getpid();
    SHELL_TERMINAL = STDIN_FILENO;
    SHELL_IS_INTERACTIVE = isatty(SHELL_TERMINAL);
    
    if (SHELL_IS_INTERACTIVE) {
        while (tcgetpgrp(SHELL_TERMINAL) != (SHELL_PGID = getpgrp())) {
            kill(SHELL_PID, SIGTTIN);
        }
        
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGINT, SIG_IGN);
        signal(SIGCHLD, &childSignalHandler);
        
        setpgid(SHELL_PID, SHELL_PID);
        SHELL_PGID = getpgrp();
        if (SHELL_PID != SHELL_PGID) {
            errormsg((char*)"Shell is not the process group leader");
            exit(EXIT_FAILURE);
        }
        if (tcsetpgrp(SHELL_TERMINAL, SHELL_PGID) == -1) {
            //TODO: tcgetattr(SHELL_TERMINAL, &SHELL_TMODES);
        }
        
    }
    else {
        errormsg((char*)"Could not make the shell interactive.");
        exit(EXIT_FAILURE);
    }
}

void childSignalHandler(int i)
{
    int status;
    pid_t pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
    if (pid > 0) {
        job* job = getJob(pid, PROCESSID);
        if (job == NULL) {
            return;
        }
        if (WIFEXITED(status)) {
            if (job->status == BACKGROUND) {
                printf("\nJob: ‰d Done\t ‰s\n", job->id, job->name);
                jobList = deleteJob(job);
            }
        }
        else if (WIFSIGNALED(status)) {
            printf("\nJob: ‰d KILLED\t %s\n", job->id, job->name);
            jobList = deleteJob(job);
        }
        else if (WIFSTOPPED(status)) {
            if (job->status == BACKGROUND) {
                tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
                setJobStatus(pid, WAITINGINPUT);
                printf("\nJob: ‰d Suspended and wants input\t ‰s\n", numberOfActiveJobs, job->name);
            }
            else {
                tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
                setJobStatus(pid, SUSPENDED);
                printf("\nJob: %d Stopped\t %s\n", numberOfActiveJobs, job->name);
            }
            return;
        }
        else {
            if (job->status == BACKGROUND) {
                jobList = deleteJob(job);
            }
        }
        tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
    }
}

int setJobStatus(int pid, int newStatus)
{
    job *job = jobList;
    if (job == NULL) {
        return 0;
    }
    else {
        int counter = 0;
        while (job != NULL) {
            if (job->pid == pid) {
                job->status = newStatus;
                return 1;
            }
            counter++;
            job = job->next;
        }
        return FALSE;
    }
}

job* deleteJob(job *j)
{
    if (jobList == NULL) {
        return NULL;
    }
    
    job* curJob = jobList->next;
    job* beforeCur = jobList;
    
    // IF first in list
    if (beforeCur->pid == j->pid) {
        beforeCur = beforeCur->next;
        numberOfActiveJobs--;
        return curJob;
    }
    
    while (curJob != NULL) {
        if (curJob->pid == j->pid) {
            numberOfActiveJobs--;
            beforeCur->next = curJob->next;
        }
        beforeCur = curJob;
        curJob = curJob->next;
    }
    return jobList;
}

job* getJob(int value, int type)
{
    job *job = jobList;
    if (type == PROCESSID) {
        while (job != NULL) {
            if (job->pid == value) {
                return job;
            }
            else {
                job = job->next;
            }
        }
    }
    else if (type == JOBID) {
        while (job != NULL) {
            if (job->id == value) {
                return job;
            }
            else {
                job = job->next;
            }
        }
    }
    else {
        return NULL;
    }
    return NULL;
}



void listJobs()
{
    job *j = jobList;
    if (j == NULL) {
        printf("\nNO ACTIVE JOBS\n");
    } else {
        printf("\nACTIVE JOBS\n");
        while (j != NULL) {
            printf("\nJob Number: %d\n", j->id);
            printf("\t- Name: %s\n", j->name);
            printf("\t- PID: %d\n", j->pid);
            printf("\t- Status: %d\n\n", j->status);
            j = j->next;
        }
    }
}

void setJobInBackground(job* j, /*int cont,*/ bool bg)
{
    if (bg) {
        if (j == NULL) {
            return;
        }
        if (/*cont && */j->status != WAITINGINPUT) {
            j->status = WAITINGINPUT;
        }
        /*if (cont) {*/
            if (kill(-j->pgid, SIGCONT) < 0) {
                perror("error");
            }
        //}
        
        tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
    }
    else {
        j->status = FOREGROUND;
        tcsetpgrp(SHELL_TERMINAL, j->pgid);
        /*if (cont) {*/
            if (kill(-j->pgid, SIGCONT) < 0) {
                perror("error");
            }
        //}
        
        waitJob(j);
        tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
    }
}

void waitJob(job* j)
{
    int status;
    while (waitpid(j->pid, &status, WNOHANG) == 0) {
        if (j->status == SUSPENDED) {
            return;
        }
    }
    jobList = deleteJob(j);
}

