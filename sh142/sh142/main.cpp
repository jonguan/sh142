//
//  main.c
//  sh142
//
//  Created by Øyvind Tangen on 25.10.11.
//  Copyright 2011 San Jose State University. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "definitions.h"
#include "pipe.h"
#include "jobs.h"
#include "history.h"
#include "main.h"


void error(char* c) {
    printf("Error: %s\n", c);
}

void printPrompt() {
    char *pathPtr = getcwd(currentPath, 1024);
    printf("%s%s ", pathPtr, promptSignature);
}

void init() {
    //SHELL_PID = getpid();
    
    // Initialize variables
    commandIdx = -1;
    command[0] = '\0';
    commandNumber = 0;
    
    currentPath = (char*) calloc(1024, sizeof(char));
    dataPath = (char*) calloc(1024, sizeof(char));
    execPath = (char*) calloc(1024, sizeof(char));
    promptSignature = (char*) calloc(16, sizeof(char));
    *promptSignature = '\0';
    
    readConfigFile();
    if (*promptSignature == '\0') promptSignature = (char*)"§";
    
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
            case 0: status = putLnToFile(configFile, (char*)"DataPath", dataPath); break;
            case 1: status = putLnToFile(configFile, (char*)"ExecPath", execPath); break;
            case 2: status = putLnToFile(configFile, (char*)"PromptSignature", promptSignature); break;
                
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
/**
 main
 @param argc count of parameters passed in
 @param argv array of strings passed in
 @returns 0 if success; any other value if fail
 */
int main (int argc, const char * argv[])

{
    init();
    while (1) {
        //TODO: getKeyPress() doesn't work properly.
        char c = fgetc(stdin);//getKeyPress();
        
        //Special case: retrieve previously entered command
        //(Yup, -17, -100 and -128 all together makes the "arrow up"-button)
        /*if (c == -17 && getKeyPress() == -100 && getKeyPress() == -128) {
            loadPreviousCommandFromHistory();
            //printf("%s", command);
            continue;
        } else */if (++commandIdx >= CMD_LEN) { //Special case: buffer is full
            error((char*)"Command buffer is full.");
            resetCommandBuffer();
            printPrompt();
            continue;
        }
        
        command[commandIdx] = c; //Assigns a char from keyboard input to command string
        
        //User hit return - replace \n with \0
        if (command[commandIdx] == '\n') { //The whole command is stored in command array
            command[commandIdx] = '\0';
            
            
            
            
            
            //Parser goes here 
            if (EXIT == parseInput(command)) {
                return 0;
            }
            
            resetCommandBuffer();
            printPrompt();
        }
    }
    return 0;
}

//TODO: This is basically ripped of teh internetz, needs rewriting.
char getKeyPress() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void resetCommandBuffer() {
    commandIdx = -1;
    for (char* c = command; *c != '\0'; c++) *c = '\0';
}

#pragma mark - Parser
int parseInput(char *inputCommand)
{
    int returnValue = UNINITIALIZED;
    char *subCommand = inputCommand; //pointer to beginning of subCommand
    char* c = inputCommand;    // iterator through cmd
    int AndExpression = UNINITIALIZED;
    
    // Break up cmd into subCommands by checking for special characters
    // for (c = inputCommand; *c != '\0'; c++) {
    while (*c != '\0') {
        // Top level checks for logical operators && and || 
        // Check for next character == null
        char *d = c+1;
        if (*d != '\0') {
            if ( ((*c == '&') && (*d == '&')) || ((*c == '|') && (*d == '|')) ) {
                //&& or ||
                
                
                
                //replace current pointer with nil and pass through
                *c = '\0';
                // evaluate subCommand
                
                if (*d == '&'){
                    // AND to old returnvalue
                   // printf("AND %s", subCommand);
                    AndExpression = TRUE;
                    returnValue = returnValue == UNINITIALIZED ? parsePipeCommand(subCommand) : returnValue || parsePipeCommand(subCommand);
                }else if (*d == '|'){
                   // printf("OR %s", subCommand);
                    AndExpression = FALSE;
                    returnValue = returnValue == UNINITIALIZED ? parsePipeCommand(subCommand) : returnValue && parsePipeCommand(subCommand);
                }
                
                
                if((returnValue != SUCCESS && *d == '&') || (returnValue == SUCCESS && *d == '|')) 
                {
                    //previous failure for AND or previous true for OR
                    // can return right away
                    break;
                }
                
                // set subCommand to next command
                c+=2;
                subCommand = c; 
                continue;
            }
        }else{
            // end of command - send all prevous to cmdInterpreter
            if (returnValue == UNINITIALIZED) {
                returnValue = parsePipeCommand(subCommand);
            }else{
                returnValue = AndExpression ? returnValue || parsePipeCommand(subCommand) : returnValue && parsePipeCommand(subCommand);                
            }
            
        }
        c++;
        
        // Set next entry of exitStatusArray
        if (returnValue == EXIT) {
            return returnValue;
        } 
    }
    
    
    
    exitStatusArray[commandNumber % NUM_REMEMBERED_CMDS] = returnValue;
    commandNumber ++;
    /*
    for (int i = 0; i < commandNumber; i++) {
        printf("%d", exitStatusArray[i]);
    }*/
    
    return returnValue;
}

int parsePipeCommand(char *command)
{
    //char* c = command;    // iterator through cmd
    //char* d = '\0'; // placeholder for 2 word arguments
    int returnValue = UNINITIALIZED;

    char *subcommand = strtok(command, "|");
    while (subcommand != NULL) {
        //send individual commands to cmdinternal
        if (returnValue == UNINITIALIZED) {
            returnValue = runExternalCommand(subcommand);
        }
        returnValue = runExternalCommand(subcommand);
        subcommand = strtok(NULL, "|");
    }
    
    /*
    while (*c != '\0') {
    //for (c = command; *c != '\0'; c++) {
        
        // exit status
        if (*c == '$' && !strncmp("$%", c, 2)) {
            c += 2;
            if (isdigit(*c)){
                char *space = strstr(c, " ");
                if (space != NULL) {
                    *space = '\0';
                    c = space+1;
                    
                }
                
                int index = atoi(subcommand);
                returnValue = getPastReturnValueAtIndex(index);
                
            }else{
                // have $%notANumber
                returnValue = EXIT_FAILURE;
            }
            subcommand = c;
        }
        
        // bg process
        else if (*c == '&'){
            // putIntoBackground(subcommand);
            *c = '\0';
            c++;
            subcommand = c;
        }
        
        // pipe
        else if (*c == '|'){
            // pass entire command to run in subfunction
            returnValue = runPipeCommand(subcommand);
            break;
        }
        
        // for 
        else if (*c == 'f' && !strncmp("for", c, 3)){
            //check rest of syntax
        }
        
        else if (*c == 'e' && !strncmp(c, "exit", 4)) {
            return EXIT;
        } else if (*c == 'P' && !strncmp(c, "PATH=", 5)) {
            //setExecPath(c + 5, end);
        } else if (*c == 'D' && !strncmp(c, "DATA=", 5)) {
            //setDataPath(cmd + 5, end);
        }
        
        else if (*c == 'j' && !strncmp("jobs", c, 4)){
            //query available jobs
        }
        else if(*(c+1) == '\0'){
            // last command
            returnValue = runSubCommand(subcommand);
        }
        
        c++; 
    }
    */
    return returnValue;
    
    
    
}
#pragma mark - Command Interpreter methods
int getPastReturnValueAtIndex(int index){
    if (index > NUM_REMEMBERED_CMDS){
        printf("Number of commands remembered is %d\n", NUM_REMEMBERED_CMDS);
    }else{
        int arraySpot = (index < commandNumber) ? (commandNumber - index) : (NUM_REMEMBERED_CMDS + commandNumber - index -1);
        printf("%d", exitStatusArray[arraySpot]);      
        return exitStatusArray[arraySpot];
    }
    return EXIT_FAILURE;
}


/*
 Checks for internal commands first, then external (from data path) later.
 return value of 0 if successful
 */
int cmdInterpreter (char* cmd) {
    
    char* c = cmd;    // iterator through cmd
    char* d = '\0'; // placeholder for 2 word arguments
   
    for (c = cmd; *c != '\0'; c++) {
        
        //check for spaces
        if (d == '\0' && *c == ' ') 
            d = c;
    }
    if (d == '\0') 
        d = c;
    
    int i = cmdInterpreterInternal(cmd, d, c);
    
    if (i == EXIT) 
        return EXIT; //Special case: exit
    else if (i != 1); //Command was handled internally
    else if (!cmdInterpreterExternal(cmd, c)); //Command was handled externally
    else { //Command was not recognized
        printf("Unknown command: '%s'\n", cmd);
        return EXIT_FAILURE;
    }
    return SUCCESS;
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
        error((char*)"Executable path was not set.");
        return 1;
    }
}

int setDataPath(char* cmd, char* end) {
    if (!setPath(cmd, end, dataPath)) {
        printf("Data path set as '%s'\n", dataPath);
        return 0;
    } else {
        error((char*)"Datapath was not set.");
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
