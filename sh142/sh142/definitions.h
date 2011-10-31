//
//  definitions.h
//  sh142
//
//  Created by Torkil Aamodt on 10/30/11.
//  Copyright 2011 Universitetet i Oslo. All rights reserved.
//

#include <unistd.h>

#ifndef sh142_definitions_h
#define sh142_definitions_h

#define CMD_LEN 128 //TODO: This value could also be stored in the config file

static char* currentPath;
static char command[CMD_LEN];
static int commandIdx;
static char* promptSignature; //TODO: Fetch from config file if there

void printPrompt();
void init();



#endif
