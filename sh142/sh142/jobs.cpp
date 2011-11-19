//
//  jobs.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 San Jose State University. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "jobs.h"


/*
bool foreground = true;

char* c = command;
char* d = c-1;
for (c = command; c != '\0'; c++) {}
if (*d == '&') {
    foreground = false;
}
*/

/*void launchJob(char *cmd) {
    bool foreground = true;
    printf("Hello\n");
    char* c;
    //char* d = c-1;
    for (c = cmd; c != '\0'; c++) {}
    if (*c == '&') {
        foreground = false;
    }
    
    pid_t pid;
    pid = fork();
    if (pid == -1) {
        perror("Shell error");
    }
    else if (pid == 0) {
        if (foreground) {
            printf("Foreground");
        } else {
            printf("Background");
        }
    } else {
        printf("I'm the father");
    }
}*/

int launchJob(char* cmd[])
{
    pid_t pid;
    int status;
    
    pid = fork();
    if(pid == 0) {
        /* Child process. */
        
        execv(cmd[0], cmd);
        
        /* If execv returns, it must have failed. */
        
        printf("Unknown command\n");
        exit(0);
    }
    else {
        /* This is run by the parent.  Wait for the child
         to terminate. */
        
        return status;
    }
}

void putIntoStruct(pid_t pid, pid_t pgid, char* name, int status) {
    
}

void putIntoForeground(char* command) {
    
}

void putIntoBackground(char* command) {
    
}