//
//  main.h
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//
#include <iostream>
#include <stdio.h>

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
int runForLoopParser(char *forLoop);
int cmdInterpreter (char* cmd);
int cmdInterpreterInternal (char* cmd);
int cmdInterpreterExternal (char* cmd);


void printPrompt(void);
void init(void);
char getKeyPress(void);
void readConfigFile(void);
void loadConfig(char str1[], int c1, char str2[], int c2);
int generateConfig(void);
int putLnToFile(FILE* dest, char* key, char* val);
void resetCommandBuffer(void);


// Environment Varaibles
int setEnvironmentVariable(char* variable, char*operand);
int setExecPath(char* cmd);
int setDataPath(char* cmd);
int setPath(char* cmd, char* p);
int validatePaths(char* pathList);

// Hash table
void add_var(struct envVar *var);
struct envVar *find_var(char* varName); 
void delete_var(struct envVar *var);
#endif
