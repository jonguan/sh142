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
    char *path = (char*)calloc(64, sizeof(char));
    //char *tmp;
    char tmp[8];
    sprintf(tmp, "%d", j.pid);
    
    strncat(path, "/proc/", 6);
    strncat(path, tmp, 8);
    strncat(path, "/stat", 5);
    
    FILE *f = fopen(path, "r");
    if (f == NULL) {
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
                totJiffies += atoi(ptr);
                break;
            case 14: //stime (CPU usage in kernel space)
                totJiffies += atoi(ptr);
                break;
                
            default:
                break;
        }
        ptr = strtok(NULL, " ");
    }
    fclose(f);
    
    f = fopen("/proc/stat", "r");
    fgets(ln, sz, f); //This is a smaller file, sz is large enough
    ptr = strtok(ln, " ");
    long totCpuTime = 0;
    
    while (ptr != NULL) { //Calculations made on a CPU core basis
        totCpuTime += atoi(ptr);
        ptr = strtok(NULL, " ");
    }
    /*while (strncmp(ln, "intr", 4)) { //ALL CPU CORES: does not work atm
        ptr = strtok(NULL, " ");
        while (ptr != NULL) {
            totCpuTime += atoi(ptr);
            ptr = strtok(NULL, " ");
        }
        fgets(ln, sz, f);
    }*/
    
    fclose(f);
    
    free(path);
    free(ln);
    
    double percentage = (double)totCpuTime / 100;
    cpuLimitJiffies = percentage * cpuLimit;
    
    //Update time
    time_t t = time(NULL);
    j.timeOverCpuLimit += difftime(t, j.lastChecked);
    j.lastChecked = t;
    
    if (totJiffies <= cpuLimitJiffies) {
        j.timeOverCpuLimit = 0;
        //printf("ALL IS WELL FOR THIS PROCESS\nLIMIT:%ld\tUSED:%ld\n", cpuLimitJiffies, totJiffies);
    } else if (j.timeOverCpuLimit > cpuTime) { //Limit broken, kill process
        //printf("DEAD\tTIMELIMIT:%d TIMESPENT:%ld\n", cpuTime, j.timeOverCpuLimit);
        killJob(j.id);
    } else {
        //printf("WARNING: TIME OVER LIMIT:%ld\tLIMIT:%ld\tUSED:%ld\n", j.timeOverCpuLimit, cpuLimitJiffies, totJiffies);
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