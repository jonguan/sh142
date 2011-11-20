//
//  main.h
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef sh142_main_h
#define sh142_main_h

#include "definitions.h"

/* VARIABLES */
static char* currentPath;
static char* dataPath;
static char* execPath;
static char command[CMD_LEN];   //User input string after we initialize (without \n)
static int commandIdx;          //Pointer to index inside string command
static char* promptSignature;
FILE *configFile;
static int exitStatusArray[NUM_REMEMBERED_CMDS];
static int commandNumber; //Points to number of executed command

/* PROTOTYPES */
//Exit status
int rememberExitStatus(int exitStatus);
void printExitStatus();
int getPastReturnValueAtIndex(int index);

//Parsing
int parseInput(char *inputCommand);
int parsePipeCommand(char *command);

void printPrompt(void);
void init(void);
char getKeyPress(void);
void readConfigFile(void);
void loadConfig(char str1[], int c1, char str2[], int c2);
int generateConfig(void);
int putLnToFile(FILE* dest, char* key, char* val);
void resetCommandBuffer(void);
int cmdInterpreter (char* cmd);
int cmdInterpreterInternal (char* cmd, char* mid, char* end);
int cmdInterpreterExternal (char* cmd, char* end);

// Environment Varaibles
int setEnvironmentVariable(char* variable, char*operand);
int setExecPath(char* cmd);
int setDataPath(char* cmd);
int setPath(char* cmd, char* p);
int validatePaths(char* pathList);


#endif
