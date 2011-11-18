//
//  definitions.h
//  sh142
//
//  Created by Torkil Aamodt on 10/30/11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include <stdio.h> /* printf, stderr, fprintf, popen, pclose, FILE */
#include <unistd.h> /* _exit, fork */
#include <string.h>
#include <stdlib.h> /* exit */
#include <sys/stat.h>
#include <errno.h> /* errno */
#include <pthread.h>
#include <termios.h>


#ifndef sh142_definitions_h
#define sh142_definitions_h

#define CMD_LEN 128 //TODO: This value could also be stored in the config file
#define NUM_REMEMEBERED_CMDS 10
#define TRUE 1
#define FALSE !TRUE
#define FOREGROUND 'F'
#define BACKGROUND 'B'
#define SUSPENDED 'S'
#define WAITING_INPUT 'W'
#define EXIT (-1)
#define UNINITIALIZED (-2)
#define SUCCESS 0

/* VARIABLES */
static char* currentPath;
static char* dataPath;
static char* execPath;
static char command[CMD_LEN];   //User input string after we initialize (without \n)
static int commandIdx;          //Pointer to index inside string command
static char* promptSignature;
FILE *configFile;
static int exitStatusArray[NUM_REMEMEBERED_CMDS];
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