//
//  jobs.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 San Jose State University. All rights reserved.
//

#include "jobs.h"

void shellInit()
{
    SHELL_PID = getpid();
    SHELL_TERMINAL = STDIN_FILENO;
    SHELL_IS_INTERACTIVE = isatty(SHELL_TERMINAL);
    
    if (SHELL_IS_INTERACTIVE) {
        /*while (tcgetpgrp(SHELL_TERMINAL) != (SHELL_PGID = getpgrp())) {
            kill(SHELL_PID, SIGCONT);
        }*/
        
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


static int numberOfActiveJobs = 0;
static job* jobList = NULL;

static pid_t SHELL_PID;
static pid_t SHELL_PGID;
static int SHELL_TERMINAL;
static int SHELL_IS_INTERACTIVE;

static struct termios SHELL_TMODES;

/**
	launchJob
	@param cmd - array of strings from command where cmd[0] = command name and all else are descriptors
	@param mode - FOREGROUND or BACKGROUND
	@param path - path to a file
	@param flag - STDIN or STDOUT
	@returns int exit status
 */
int launchJob(char* cmd[], int mode, char* path, int flag)

{
    pid_t pid = fork();
    
    if (pid == -1) { //Error forking
        errormsg((char*)"Failed to fork new process");
        return EXIT_FAILURE;
    } else if (pid == 0) { //Child only
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, &childSignalHandler);
        signal(SIGTTIN, SIG_DFL);
        
        usleep(20000);
        setpgrp();
        
        if (mode == FOREGROUND) {tcsetpgrp(SHELL_TERMINAL, getpid());}
        else if (mode == BACKGROUND) {
            numberOfActiveJobs++;
            printf("Job [%d] launched as process '%d'\n",numberOfActiveJobs, getpid());
        }
        
        //Run the job
        int descriptor;
        if (flag == STDIN) {
            descriptor = open(path, O_RDONLY, 0600);
            dup2(descriptor, STDIN_FILENO); //when running cmd, will take path as input
            close(descriptor);
        }
        if (flag == STDOUT) {
            descriptor = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            dup2(descriptor, STDOUT_FILENO); //when running cmd, will write out to path
            close(descriptor);
        }
        if (execvp(*cmd, cmd) == -1){
            perror("Failed to execute job");
            //exit(EXIT_FAILURE);
            return EXIT_FAILURE;
        }
        
        //exit(EXIT_SUCCESS);
        return EXIT_SUCCESS;
    } else { //Parent only
        
        setpgid(pid, pid); //Will default to pid
        
        jobList = addJob(pid, pid, *cmd, path, mode);
        job *j = getJob(pid, PROCESSID);
        //printf("TEST1\n");
        switch (mode) {
            case FOREGROUND: jobToForeground(j, FALSE); break;
            case BACKGROUND: jobToBackground(j, FALSE); break;
            //case FOREGROUND: setJobInBackground(j, 0, false); break;
            //case BACKGROUND: printf("TEST\n"); setJobInBackground(j, 0, true); break;
            default: break;
        }
        wait(NULL);
    }
    
    
    
    
    return EXIT_SUCCESS;
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
                printf("\nJob: %d Done\t %s\n", j->id, j->name);
                jobList = deleteJob(j);
            }
        }
        else if (WIFSIGNALED(status)) {
            printf("\nJob: %d KILLED\t %s\n", j->id, j->name);
            jobList = deleteJob(j);
        }
        else if (WIFSTOPPED(status)) {
            if (j->status == BACKGROUND) {
                tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
                setJobStatus(pid, WAITINGINPUT);
                printf("\nJob: %d Suspended and wants input\t %s\n", numberOfActiveJobs, j->name);
            }
            else {
                tcsetpgrp(SHELL_TERMINAL, j->pgid);
                setJobStatus(pid, SUSPENDED);
                printf("\nJob: %d Stopped\t %s\n", numberOfActiveJobs, j->name);
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
    job *j = jobList;
    if (j == NULL) {
        return 0;
    }
    else {
        int i = 0;
        while (j != NULL) {
            if (j->pid == pid) {
                j->status = newStatus;
                return 1;
            }
            i++;
            j = j->next;
        }
        return FALSE;
    }
}

job* deleteJob(job *j)
{
    
    usleep(10000);
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
    sleep(10000);
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
    else if (type == JOBSTATUS) {
        while (job != NULL) {
            if (job->status == value) {
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
            printf("\t- Status: %d\n", j->status);
            printf("\t- Descriptor: %s\n\n", j->descriptor);
            j = j->next;
        }
    }
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
        
        waitJob(j);
        tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
    }
}

void jobToForeground(job *j, int cont) {
    j->status = FOREGROUND;
    tcsetpgrp(SHELL_TERMINAL, j->pgid);
    if (cont) {
        if (kill(-j->pgid, SIGCONT) < 0) {
            perror("kill (SIGCONT)");
        }
    }
    waitJob(j);
    tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
}

void jobToBackground(job *j, int cont) {
    if (j == NULL) {
        return;
    }
    if (cont && j->status == WAITINGINPUT) {
        j->status = WAITINGINPUT;
    }
    if (cont) {
        if (kill(-j->pgid, SIGCONT) < 0) {
            perror("kill (SIGCONT)");
        }
    }
    
    tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
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

void killJob(int id)
{
    job *j = getJob(id, JOBID);
    if (id == NULL || id > numberOfActiveJobs) {
        return;
    }
    kill(j->pid, SIGKILL);
}

