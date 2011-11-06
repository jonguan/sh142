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
    dataPath = (char*) calloc(1024, sizeof(char));
    execPath = (char*) calloc(1024, sizeof(char));
    promptSignature = (char*) calloc(16, sizeof(char));
    
    readConfigFile();


    promptSignature = "§";

    printPrompt();
}

void readConfigFile()
{
    configFile = fopen(".sh142", "r");
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
        dataPath = value;
    }
    else if (strcmp(config, "ExecPath") == 0) {
        execPath = value;
    }
    /*else if (strcmp(config, "CmdLength") == 0) {
        CMD_LEN = atoi(value);
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
            
            printf("You entered: '%s'\n", command); //This should be replaced with a call to command interpreter
            cmdInterpreter(command);
            
            commandIdx = -1; //Resets command array
            printPrompt();
        } else {
            
        }
    }
    
    return 0;
}

/*
 Checks for internal commands first, then external (from data path) later.
 */
int cmdInterpreter (char* cmd) {
    char* c;
    for (c = cmd; *c != '\0'; c++);
    if (cmdInterpreterInternal(cmd, c)) return 1;
    else if (cmdInterpreterExternal(cmd, c)) return 1;
    else {
        printf("Unknown command: '%s'\n", cmd);
        return 0;
    }
}

/*
 Add new internal commands here.
 If the command takes agruments, remember to check if
 range == length of command name (i.e range == 2 for cd or ls).
 
 Returns 1 if command was processed, else 0.
 */
int cmdInterpreterInternal (char* cmd, char* end) {
    long range = end - cmd;
    if (!strncmp(cmd, "PATH=", 5)) {
        if (!setExecPath(cmd, end))
            printf("Error: Failed to set executable path");
    } else if (!strncmp(cmd, "DATA=", 5)) {
        if (!setDataPath(cmd, end))
            printf("Error: Failed to set data path");
    } else if (range == 8 && !strncmp(cmd, "example1", range)) { //template example
        printf("echo 1\n");
        return 1;
    } else if (range == 8 && !strncmp(cmd, "example2", range)) { //template example
        printf("echo 2\n");
        return 1;
    } else if (range == 8 && !strncmp(cmd, "example3", range)) { //template example
        printf("echo 3\n");
        return 1;
    } else return 0; //Command is not reckognized as internal
    
    return 1; //Command was processed as internal
}

int cmdInterpreterExternal (char* cmd, char* end) {
    return 0;
}

int setExecPath(char* cmd, char* end) { //TODO: Save to config file
    if (setPath(cmd, end, execPath)) {
        printf("Executable path set as '%s'\n", execPath);
        return 1;
    }
    return 0;
}

int setDataPath(char* cmd, char* end) { //TODO: Save to config file
    if (setPath(cmd, end, dataPath)) {
        printf("Data path set as '%s'\n", dataPath);
        return 1;
    }
    return 0;
}

int setPath(char* cmd, char* end, char* p) {
    if (end - cmd > 1024) {
        return 0;
    }
    
    char* pathPtr = p;
    for (char* c = cmd + 5; c != end; c++) {
        *pathPtr = *c;
        pathPtr++;
    }
    *pathPtr = '\0';
    return 1;
}
