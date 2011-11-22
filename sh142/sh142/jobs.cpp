//
//  jobs.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 San Jose State University. All rights reserved.
//

#include "jobs.h"

static job* jobList = NULL;

void shellInit()
{
    SHELL_PID = getpid();
    SHELL_TERMINAL = STDIN_FILENO;
    
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
        tcgetattr(SHELL_TERMINAL, &SHELL_TMODES);
    }
}

/**
 launchJob
 @param cmd - array of strings from command where cmd[0] = command name and all else are descriptors
 @param mode - FOREGROUND or BACKGROUND
 @param path - path to a file
 @param flag - STDIN or STDOUT
 @returns int exit status
 */
int launchJob(char* cmd[], char* path, int flag, int mode)
{
    pid_t pid = fork();
    if (pid == -1) {
        errormsg((char*)"Failed to fork new process");
        return -1;
    }
    else if (pid == 0) { // INSIDE CHILD
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_IGN);//
        signal(SIGCHLD, &childSignalHandler);
        usleep(20000);
        setpgrp();
        if (mode == FOREGROUND) {
            tcsetpgrp(SHELL_TERMINAL, getpid());
        }
        if (mode == BACKGROUND) {
            numberOfActiveJobs++;
            printf("Job: %d\tPID: %d\n", numberOfActiveJobs, (int) getpid());
            tcsetpgrp(SHELL_TERMINAL, getpid());
        }
        
        // Run the job
        int descriptor;
        if (flag == STDIN) {
            descriptor = open(path, O_RDONLY, 0600);
            dup2(descriptor, STDIN_FILENO);
            close(descriptor);
        }
        if (flag == STDOUT) {
            descriptor = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            dup2(descriptor, STDOUT_FILENO);
            close(descriptor);
        }
        
        if (execvp(*cmd, cmd) == -1) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    else {
        setpgid(pid, pid);
        jobList = addJob(pid, pid, *cmd, path, mode);
        
        job *j = getJob(pid, PROCESSID);
        
        switch (mode) {
            case FOREGROUND: printf("%s Launched in foreground\n", j->name); setJobInBackground(j, 0, false); break;
            case BACKGROUND: printf("%s Launched in background\n", j->name); setJobInBackground(j, 0, true); break;
            default: break;
        }
    }
    tcsetpgrp(SHELL_TERMINAL, getpid());
    return EXIT_SUCCESS;
}

void setJobInBackground(job* j, int cont, bool bg)
{
    if (bg) {
        if (j == NULL) {
            return;
        }
        if (cont && j->status != WAITINGINPUT) {
            j->status = WAITINGINPUT;
        }
        if (cont) {
            if (kill(-j->pgid, SIGCONT) < 0) {
                perror("error");
            }
        }
        usleep(10000);
        tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
    }
    else {
        j->status = FOREGROUND;
        tcsetpgrp(SHELL_TERMINAL, j->pgid);
        if (cont) {
            if (kill(-j->pgid, SIGCONT) < 0) {
                perror("error");
            }
        }
        
        waitForJob(j);
        tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
    }
}


void errormsg(char* c) {
    printf("Error: %s\n", c);
}

job* addJob(pid_t pid, pid_t pgid, char* jobName,char* descriptor, int status)
{
    usleep(10000);
    job *j = (job*) malloc(sizeof(job));
    j->name = (char*) malloc(sizeof(jobName));
    j->name = strcpy(j->name, jobName);
    j->pid = pid;
    j->pgid = pgid;
    j->lastChecked = time(NULL);
    j->timeOverCpuLimit = 0;
    j->status = status;
    j->descriptor = (char*) malloc(sizeof(descriptor));
    j->descriptor = strcpy(j->descriptor, descriptor);
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

void childSignalHandler(int i)
{
    int status;
    pid_t pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
    if (pid > 0) {
        job* j = getJob(pid, PROCESSID);
        if (j == NULL) {
            return;
        }
        if (WIFEXITED(status)) {
            if (j->status == BACKGROUND) {
                printf("\nJob %d (%s) Done\n", j->id, j->name);
                jobList = deleteJob(j);
            }
        }
        else if (WIFSIGNALED(status)) {
            printf("\nJob %d (%s) Killed\n", j->id, j->name);
            jobList = deleteJob(j);
        }
        else if (WIFSTOPPED(status)) {
            if (j->status == BACKGROUND) {
                tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
                setJobStatus(pid, WAITINGINPUT);
                printf("\nJob %d (%s) Suspended and waiting input\n", j->id/* numberOfActiveJobs*/, j->name);
            }
            else {
                tcsetpgrp(SHELL_TERMINAL, j->pgid);
                setJobStatus(pid, SUSPENDED);
                printf("\nJob %d (%s) Stopped\n", j->id/* numberOfActiveJobs*/, j->name);
            }
            return;
        }
        else {
            if (j->status == BACKGROUND) {
                jobList = deleteJob(j);
            }
        }
        tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
    }
}

int setJobStatus(int pid, int newStatus)
{
    usleep(10000);
    if (jobList == NULL) {
        return 0;
    }
    else {
        job* j = getJob(pid, PROCESSID);
        j->status = newStatus;
    }
    return 0;
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
    usleep(10000);
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

job* getJobList() {
    return jobList;
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
            printf("\t- Status: %c\n", j->status);
            printf("\t- Descriptor: %s\n\n", j->descriptor);
            j = j->next;
        }
    }
}


void waitForJob(job* j)
{
    int status;
    while (waitpid(j->pid, &status, WNOHANG) == 0) {
        if (j->status == SUSPENDED) {
            return;
        }
    }
    jobList = deleteJob(j);
}

void killJob(int id)
{
    job *j = getJob(id, JOBID);
    if (j == NULL || id > numberOfActiveJobs) {
        return;
    }
    kill(j->pid, SIGKILL);
}
