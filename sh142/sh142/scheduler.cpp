//
//  scheduler.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/20/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "scheduler.h"

int checkOnProcess(pid_t pid) {
    char *path = (char*)calloc(64, sizeof(char));
    //char *tmp;
    char tmp[8];
    sprintf(tmp, "%d", pid);
    
    strncat(path, "/proc/", 6);
    strncat(path, tmp, 8);
    strncat(path, "/stat", 5);
    
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        printf("File '%s' doesn't exist.\nAre you on a Mac?\n", path);
        free(path);
        return EXIT_FAILURE;
    }
    fseek(f, 0L, SEEK_END);
    int sz = (int)ftell(f);
    fseek(f, 0L, SEEK_SET);
    
    char* ln = (char*)calloc(sz, sizeof(char));
    fgets(ln, sz, f);
    printf("%d", sz);
    /*while (*ln != '\0') {
        fgets(ln, 256, f);
        printf("%s", ln);
    }*/
    
    free(path);
    free(ln);
    
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
    
    CPUPercentage = percentage;
    CPUSeconds = seconds;
    
    return EXIT_SUCCESS;
}