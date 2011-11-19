//
//  pipe.h
//  sh142
//
//  Created by Jonathan.Guan on 11/12/11.
//  Copyright (c) 2011 Universitetet i Oslo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"

#ifndef sh142_pipe_h
#define sh142_pipe_h

int runExternalCommand(char*);
int runSubCommand(char*subCommand);
int runPipeCommand(char* inCommand, char* outCommand);
int runPipeReadCommand(char *command, char*result); // read from stdin, writes to result
int runPipeWriteCommand(char *command, char* inputString, char* result);//Reads from inputFile, passes to command, writes into result
#endif
