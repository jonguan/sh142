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

int runPipeParser(char* pipe);
int runExternalCommand(char*);
int runSubCommand(char*subCommand);
int runPipeCommand(char* inCommand, char* outCommand);
int pipeCommand(char* cmd1[], char* cmd2[]);
int runPipeReadCommand(char *command, char*file); // read from stdin, writes to result
int runPipeWriteCommand(char *command, char* fileIn, char* fileOut);//Reads from inputFile, passes to command, writes into result

// Redirect STDIN and STDOUT
int redirectToFile (char *command);

// Utility
int copyFromFile(char*destination, char*source);
#endif
