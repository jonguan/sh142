//
//  main.c
//  sh142
//
//  Created by Øyvind Tangen on 25.10.11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include <stdio.h>
#include "definitions.h"

void printPrompt() {
    char *pathPtr = getcwd(currentPath, 1024);
    printf("%s%s ", pathPtr, promptSignature);
}

void init() {
    commandIdx = -1;
    command[0] = '\0';
    
    currentPath = (char*) calloc(1024, sizeof(char));
    
    promptSignature = " :-)";
    printPrompt();
}

int main (int argc, const char * argv[])
{
    init();
    
    while (1) {
        char c = fgetc(stdin);
        if (++commandIdx >= CMD_LEN) {
            printf("Error: Command buffer is full.\n");
            command[0] = '\0';
            commandIdx = -1;
            printPrompt();
            continue;
        }
        
        command[commandIdx] = c; //Assigns a char from keyboard input to command string
        
        if (command[commandIdx] == '\n') { //The whole command is stored in command array
            command[commandIdx] = '\0';
            
            printf("%s\n", command); //This should be replaced with a call to command interpreter
            
            commandIdx = -1; //Resets command array
            printPrompt();
        } else {
            
        }
    }
    
    return 0;
}

