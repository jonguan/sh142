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

//static pid_t SHELL_PID;
//static pid_t SHELL_PGID;
//static int SHELL_TERMINAL;
//static int SHELL_IS_INTERACTIVE;
//struct termios SHELL_TMODES;

/* PROTOTYPES */
int parseInput(char *inputCommand);
void error(char* c);
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


int setExecPath(char* cmd, char* end);
int setDataPath(char* cmd, char* end);
int setPath(char* cmd, char* end, char* p);
int validatePaths(char* pathList);

void launchJob(char *command[]);

#endif
