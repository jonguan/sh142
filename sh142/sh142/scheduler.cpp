//
//  scheduler.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/20/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "scheduler.h"

void *restrictProcesses(void* param) {
    while (1) {
        sleep(3);
        job *j = getJobList();
        if (j == NULL) {
            continue;
        }
        restrictProcesses(*j);
    }
}

void restrictProcesses(job j) {
    job *ptr;
    for (; ptr != NULL; ptr = ptr->next) {
        checkOnProcess(*ptr);
    }
}

/*
 * Checks if a process uses more resources over time than it should.
 * Kills process if it does.
 * CPU% is relative to all processors/cores in system.
 */
int checkOnProcess(job j) {
    //printf("MEMMAX:%d\tMEMTIME:%d\n", memLimit, memTime);
    if (!isInitialized) {
        isInitialized = initScheduler() == 0;
        if (!isInitialized) {
            return 1;
        }
    }
    
    char *ppath = (char*)calloc(64, sizeof(char));
    //char *tmp;
    char tmp[8];
    sprintf(tmp, "%d", j.pid);
    
    strncat(ppath, "/proc/", 6);
    strncat(ppath, tmp, 8);
    char *path = (char*)calloc(64, sizeof(char));
    strcpy(path, ppath);
    strncat(path, "/stat", 5);
    
    if (fileExists(path)) {
        free(ppath);
        free(path);
        return 1;
    }
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        free(ppath);
        free(path);
        return EXIT_FAILURE;
    }
    fseek(f, 0L, SEEK_END);
    int sz = (int)ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    char* ln = (char*)calloc(sz, sizeof(char));
    fgets(ln, sz, f);
    
    char* ptr = strtok(ln, " ");
    long totJiffies = 0;
    for (int i = 0; ptr != NULL; i++) {
        switch (i) {
            case 13: //utime (CPU usage in user space)
                totJiffies += atol(ptr);
                break;
            case 14: //stime (CPU usage in kernel space)
                totJiffies += atol(ptr);
                break;
                
            default:
                break;
        }
        ptr = strtok(NULL, " ");
    }
    fclose(f);
    
    double percentage = (double)totCpuTime / 100;
    cpuLimitJiffies = percentage * cpuLimit;
    
    //Update time
    time_t t = time(NULL);
    
    time_t delta = difftime(t, j.lastChecked);
    j.timeAlive += delta;
    j.timeOverCpuLimit += delta;
    j.lastChecked = t;
    
    //printf("usedJiffies:%ld\tlimit:%ld\n", totJiffies, cpuLimitJiffies);
    
    if (totJiffies <= cpuLimitJiffies) {
        j.timeOverCpuLimit = 0;
        //printf("ALL IS WELL FOR THIS PROCESS\nLIMIT:%ld\tUSED:%ld\n", cpuLimitJiffies, totJiffies);
    } else if (j.timeOverCpuLimit > cpuTime) { //Limit broken, kill process
        //printf("DEAD\tTIMELIMIT:%d TIMESPENT:%ld\n", cpuTime, j.timeOverCpuLimit);
        killJob(j.id);
        free(ppath);
        free(path);
        free(ln);
        return EXIT_SUCCESS;
    } else {
        //printf("WARNING: TIME OVER LIMIT:%ld\tLIMIT:%ld\tUSED:%ld\n", j.timeOverCpuLimit, cpuLimitJiffies, totJiffies);
    }
    
    if (j.timeAlive > timeLimitSeconds) {
        //printf("PROCESS EXCEEDED TIME LIMIT: %ld>%ld\n", j.timeAlive, timeLimitSeconds);
        killJob(j.id);
        free(ppath);
        free(path);
        free(ln);
        return EXIT_SUCCESS;
    }
    
    long totMemory = 0; //Memory spent by process
    strcpy(path, ppath);
    strcat(path, "/statm");
    
    if (fileExists(path)) {
        free(ppath);
        free(path);
        free(ln);
        return 1;
    }
    f = fopen(path, "r");
    if (f == NULL) {
        free(ppath);
        free(path);
        free(ln);
        return EXIT_FAILURE;
    }
    fseek(f, 0L, SEEK_END);
    sz = (int)ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    fgets(ln, sz, f);
    
    ptr = strtok(ln, " ");
    totMemory = atol(ptr);
    
    fclose(f);
    
    if (totMemory > memLimitJiffies) {
        if (j.timeOverMemLimit > memTime) {
            killJob(j.id);
            free(ppath);
            free(path);
            free(ln);
            return EXIT_SUCCESS;
        }
    } else {
        j.timeOverMemLimit = 0;
    }
    
    free(ppath);
    free(path);
    free(ln);
    
    return EXIT_SUCCESS;
}

long initScheduler() {
    if (fileExists((char*)"/proc/stat")) {
        return 1;
    }
    FILE *f = fopen("/proc/stat", "r");
    if (f == NULL) {
        return EXIT_FAILURE;
    }
    fseek(f, 0L, SEEK_END);
    int sz = (int)ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    char* ln = (char*)calloc(sz, sizeof(char));
    char* ptr = ln;
    
    fgets(ln, sz, f);
    ptr = strtok(ln, " ");
    
    while (ptr != NULL) { //Calculations made on a CPU core basis
        totCpuTime += atol(ptr);
        ptr = strtok(NULL, " ");
    }
    fclose(f);
    
    if (fileExists((char*)"/proc/meminfo")) {
        free(ln);
        return 1;
    }
    f = fopen("/proc/meminfo", "r");
    if (f == NULL) {
        free(ln);
        return EXIT_FAILURE;
    }
    /*fseek(f, 0L, SEEK_END);
    sz = (int)ftell(f);
    fseek(f, 0L, SEEK_SET);*/
    sz = 256;
    
    ln = (char*)calloc(sz, sizeof(char));
    ptr = ln;
    
    fgets(ln, sz, f);
    ptr = strtok(ln, " ");
    ptr = strtok(NULL, " ");
    totMemTime = atol(ptr);
    
    fclose(f);
    free(ln);
    
    return 0;
}

int setTimeMax(char* cmd) {
    char* c = cmd + 8;
    int tLimit;
    if (sscanf(c, "%d", &tLimit) == EOF) {
        return EXIT_FAILURE;
    }
    
    timeLimitSeconds = tLimit;
    
    return EXIT_SUCCESS;
}

int setMemMax(char* cmd) {
    char* buffer = (char*)calloc(32, sizeof(char));
    char* c = (char*)calloc(32, sizeof(char));
    c = cmd + 7;
    strcpy(buffer, c);
    int megabytes, seconds;
    
    char* ptr = strstr(c, "M:");
    if (ptr == NULL) {
        return EXIT_FAILURE;
    }
    
    
    /**ptr = '\0';
    ptr++;
    *ptr = '\0';
    ptr++;*/
    
    ptr = buffer;
    ptr = strtok(buffer, "M:");
    
    megabytes = atoi(ptr);
    if (megabytes == 0) {
        return EXIT_FAILURE;
    }
    ptr = strtok(NULL, "M:");
    seconds = atoi(ptr);
    if (seconds == 0) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int setCpuMax(char* cmd) {
    char* c = cmd + 7;
    int percentage, seconds;
    
    c = strtok(c, ":");
    if (c == NULL) {
        return EXIT_FAILURE;
    } else if (sscanf(c, "%d", &percentage) == EOF) {
        return EXIT_FAILURE;
    }
    c = strtok(NULL, ":");
    if (c == NULL) {
        return EXIT_FAILURE;
    } else if (sscanf(c, "%d", &seconds) == EOF) {
        return EXIT_FAILURE;
    } else if (strtok(NULL, ":") != NULL) {
        return EXIT_FAILURE;
    }
    
    cpuLimit = percentage;
    cpuTime = seconds;
    
    return EXIT_SUCCESS;
}

int fileExists(char* path) {
    struct stat s;
    if (stat(path, &s) == 0) {
        return 0;
    }
    return 1;
}