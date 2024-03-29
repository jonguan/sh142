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
//#include <termios.h>
#include <ctype.h> /* isdigit, isalpha*/
//#include <curses.h> /* true and false*/
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <signal.h>
#include "uthash.h"

#ifndef sh142_definitions_h
#define sh142_definitions_h

#define CMD_LEN 128 //TODO: This value could also be stored in the config file. Or could it?
#define CMD_HISTORY_LEN 30
#define NUM_REMEMBERED_CMDS 10
#define TRUE 1        //TRUE and FALSE already defined in curses.h
#define FALSE !TRUE
#define EXIT (-1)
#define UNINITIALIZED (-2)
#define SIZE_PIPE_BUFFER 5000


#endif