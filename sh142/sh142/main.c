//
//  main.c
//  sh142
//
//  Created by Øyvind Tangen on 25.10.11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "definitions.h"

void printPrompt() {
    char *pathPtr = getcwd(currentPath, 1024);
    printf("%s%s ", pathPtr, promptSignature);
}

void init() {
    readConfigFile();
    commandIdx = -1;
    command[0] = '\0';
    
    currentPath = (char*) calloc(1024, sizeof(char));
    
    promptSignature = " :-)";
    printPrompt();
}

void readConfigFile()
{
    configFile = fopen("sh", "r");
    if (configFile != NULL) {
        //printf("CONFIG FILE FOUND\n");
		char str1[128];
		char str2[128];
		int c1;
		int c2;
        int counter = 0;
		int first = 1;
        while(!feof(configFile)) {
            char c = getc(configFile);
			if (c == '\n') {
				c2 = counter;
				counter = 0;
				loadConfig(str1, c1, str2, c2);
				memset(str1, 0, sizeof(str1));
				memset(str2, 0, sizeof(str2));
				first = 1;
				counter = 0;
			}
			if (c == ' ') {
				first = 0;
				c1 = counter;
				counter = 0;
			}
			if (first == 1) {
				if (c != '\n' && c != ' ') {
					str1[counter] = c;
					counter++;
				}
			} else {
				if (c != '\n' && c != ' ') {
					str2[counter] = c;
					counter++;
				}
			}
        }
        
    }
	fclose(configFile);
}


void loadConfig(char str1[], int c1, char str2[], int c2) {
	char config[c1];
	char value[c2];
	int i;
	for (i = 0; i < c1; i++) {
		config[i] = str1[i];
	}
	
	for (i = 0; i < c2; i++) {
		value[i] = str2[i];
	}
	
	if (strcmp(config, "PromptSignature") == 0) {
		promptSignature = value;
	}
    else if (strcmp(config, "DataPath") == 0) {
        //dataPath = value;
    }
    else if (strcmp(config, "ExecPath") == 0) {
        //execPath = value;
    }
    /*else if (strcmp(config, "CONFIG") == 0) {
        VALUE = value;
    }*/
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

