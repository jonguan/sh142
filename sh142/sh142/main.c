//
//  main.c
//  sh142
//
//  Created by Øyvind Tangen on 25.10.11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include <stdio.h>
#include "definitions.h"
#include "pipe.h"

void error(char* c) {
    printf("Error: %s\n", c);
}

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
    *promptSignature = '\0';
    
    readConfigFile();
    if (*promptSignature == '\0') promptSignature = "§";
    
    printPrompt();
}

#pragma mark - Configuration methods

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

int generateConfig() {
    configFile = fopen(".sh142", "w");
    int loop = 1, status;
    for (int i = 0; loop; i++) {
        switch (i) {
            case 0: status = putLnToFile(configFile, "DataPath", dataPath); break;
            case 1: status = putLnToFile(configFile, "ExecPath", execPath); break;
            case 2: status = putLnToFile(configFile, "PromptSignature", promptSignature); break;
                
            default:loop = 0; break;
        }
        if (status) {
            return 1;
        }
    }
    fclose(configFile);
    return 0;
}

int putLnToFile(FILE* dest, char* key, char* val) {
    if (fputs(key, dest) &&
        fputc(' ', dest) &&
        fputs(val, dest) &&
        fputc('\n', dest)) {
        return 0;
    }
    return 1;
}


#pragma mark - Main Method
int main (int argc, const char * argv[])
{
    init();
    while (1) {
        char c = fgetc(stdin);
        if (++commandIdx >= CMD_LEN) {
            error("Command buffer is full.");
            resetCommandBuffer();
            printPrompt();
            continue;
        }
        
        command[commandIdx] = c; //Assigns a char from keyboard input to command string
        
        if (command[commandIdx] == '\n') { //The whole command is stored in command array
            command[commandIdx] = '\0';
            
            if (cmdInterpreter(command) == -1) break;
            
            resetCommandBuffer();
            printPrompt();
        } else {
            //TODO: Add case for "up arrow" pressed (get previous command entered)
        }
    }
    
    return 0;
}

void resetCommandBuffer() {
    commandIdx = -1;
    for (char* c = command; *c != '\0'; c++) *c = '\0';
}


#pragma mark - Command Interpreter methods
/*
 Checks for internal commands first, then external (from data path) later.
 */
int cmdInterpreter (char* cmd) {
    char* c;
    char* d = '\0';
    for (c = cmd; *c != '\0'; c++) {
        if (d == '\0' && *c == ' ') d = c;
    }
    if (d == '\0') d = c;
    int i = cmdInterpreterInternal(cmd, d, c);
    
    if (i == -1) return -1; //Special case: exit
    else if (i != 1); //Command was handled internally
    else if (!cmdInterpreterExternal(cmd, c)); //Command was handled externally
    else { //Command was not recognized
        printf("Unknown command: '%s'\n", cmd);
        return 1;
    }
    return 0;
}

/*
 Attempts to apply a command internally.
 (Add new internal commands in this method.)
 
 cmd points to first char in command.
 mid points to the first char after the command name.
 end points to the last char of the command.
 
 Returns 0 if command was processed, else 1.
 */
int cmdInterpreterInternal (char* cmd, char* mid, char* end) {
    long range = mid - cmd;
    if (end == cmd) {
    } else if (range == 4 && !strncmp(cmd, "exit", 4)) {
        return -1;
    } else if (!strncmp(cmd, "PATH=", 5)) {
        setExecPath(cmd + 5, end);
    } else if (!strncmp(cmd, "DATA=", 5)) {
        setDataPath(cmd + 5, end);
    } else if (range == 5 && !strncmp(cmd, "test1", range)) { //template example
            printf("echo 1\n");
    } else if (range == 5 && !strncmp(cmd, "test2", range)) { //template example
            printf("echo 2 with parameters: '%s'.\n", mid + 1);
    } else return 1;
    return 0; //Command was processed as internal
}

int cmdInterpreterExternal (char* cmd, char* end) {
    
    /*
    char *ptr;
    char *startPtr = execPath;
    for (ptr = execPath; *ptr != '\0'; ptr++) {
        if (*ptr == ':') {
            *ptr = '\0';
            printf("%s\n", startPtr); //TODO: Match cmd to path contents
            startPtr = ptr + 1;
            *ptr = ':';
        }
    }
    printf("%s\n", startPtr);
    return 1;
     */
    return runExternalCommand(cmd);

}

int setExecPath(char* cmd, char* end) {
    if (!setPath(cmd, end, execPath)) {
        printf("Executable path set as '%s'\n", execPath);
        return 0;
    } else {
        error("Executable path was not set.");
        return 1;
    }
}

int setDataPath(char* cmd, char* end) {
    if (!setPath(cmd, end, dataPath)) {
        printf("Data path set as '%s'\n", dataPath);
        return 0;
    } else {
        error("Datapath was not set.");
        return 1;
    }
}

int setPath(char* cmd, char* end, char* p) {
    if (end - cmd > 1024 || validatePaths(cmd)) return 1;
    
    char* pathPtr = p;
    for (char* c = cmd; c != end; c++) {
        *pathPtr = *c;
        pathPtr++;
    }
    *pathPtr = '\0';
    generateConfig();
    return 0;
}

int validatePaths(char* pathList) {
    int foundErrors = 0;
    char* ptr;
    char* ptrStart = pathList;
    struct stat s;
    for (ptr = pathList; *ptr != '\0'; ptr++) {
        if (*ptr == ':') {
            *ptr = '\0'; //Temporarily cuts array short
            if(stat(ptrStart, &s)) {
                printf("Error: Path '%s' does not exist.\n", ptrStart);
                foundErrors++;
            }
            *ptr = ':';
            ptrStart = ptr + 1;
        }
    }
    if(stat(ptrStart, &s)) {
        printf("Error: Path '%s' does not exist.\n", ptrStart);
        foundErrors++;
    }
    return foundErrors;
}

