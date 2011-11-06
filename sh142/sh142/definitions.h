//
//  definitions.h
//  sh142
//
//  Created by Torkil Aamodt on 10/30/11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifndef sh142_definitions_h
#define sh142_definitions_h

#define CMD_LEN 128 //TODO: This value could also be stored in the config file

/* VARIABLES */
static char* currentPath;
static char* dataPath;
static char* execPath;
static char command[CMD_LEN];
static int commandIdx;
static char* promptSignature; //TODO: Fetch from config file if there
FILE *configFile;

/* PROTOTYPES */
void printPrompt(void);
void init(void);
void readConfigFile(void);
void loadConfig(char str1[], int c1, char str2[], int c2);
int cmdInterpreter (char* cmd);
int cmdInterpreterInternal (char* cmd, char* end);
int cmdInterpreterExternal (char* cmd, char* end);
int setExecPath(char* cmd, char* end);
int setDataPath(char* cmd, char* end);
int setPath(char* cmd, char* end, char* p);

#endif