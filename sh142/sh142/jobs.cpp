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

int launchJob(char* cmd[], int mode, char* path, int flag)
{
    pid_t pid = fork();
    
    if (pid == -1) { //Error forking
        errormsg((char*)"Failed to fork new process");
    } else if (pid == 0) { //Child only
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGINT, SIG_DFL);
        signal(SIGCHLD, &childSignalHandler);
        //usleep(20000);
        setpgrp();
        
        if (mode == FOREGROUND) {tcsetpgrp(SHELL_TERMINAL, getpid());}
        else if (mode == BACKGROUND) {
            printf("Job launched as process '%d'\n", getpid());
            numberOfActiveJobs++;
        }
        
        //Run the job
        /*int descriptor;
        if (flag == STDIN) {
            descriptor = open(path, O_RDONLY, 0600);
            dup2(descriptor, STDIN_FILENO);
            close(descriptor);
        }
        if (flag == STDOUT) {
            descriptor = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            dup2(descriptor, STDOUT_FILENO);
            close(descriptor);
        }*/
        if (execvp(*cmd, cmd) == -1) perror("Failed to execute job");
        
        exit(EXIT_SUCCESS);
        
    } else { //Parent only
        
        setpgid(0, 0); //Will default to pid
        if (getpgid(pid) != pid) { //TODO: Remove if when it works
            printf("Change call 'setpgid(0, 0)' to 'setpgid(pid, pid)' in launchJob()");
        }
        
        jobList = addJob(pid, pid, *cmd, mode);
        job *j = getJob(pid, PROCESSID);
        switch (mode) {
            //case FOREGROUND: setJobInBackground(j, /*continue, */false); break;
            //case BACKGROUND: setJobInBackground(j, /*continue, */true); break;    
            default: break;
        }
        
    }
    
    
    
    
    return 1;
}

void errormsg(char* c) {
    printf("Error: %s\n", c);
}

job* addJob(pid_t pid, pid_t pgid, char* jobName, int status) {
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


int putIntoForeground(job* j) {
    
    return 0;
}

int putIntoBackground(job* j) {
    
    
    
    tcsetpgrp(SHELL_TERMINAL, j->pgid);
    return 0;
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
            tcgetattr(SHELL_TERMINAL, &SHELL_TMODES);
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

int setJobStatus(int pid, int newStatus) {
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

job* deleteJob(job *j) {
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

job* getJob(int value, int type) {
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


/*
void listJobs()
{
    job *j = jobList;
    if (j == NULL) {
        printf("\nNO ACTIVE JOBS\n");
    } else {
        printf("\nACTIVE JOBS\n");
        while job != NULL) {
            printf("Job name: %7s 
        }
    }
    
}
*/






