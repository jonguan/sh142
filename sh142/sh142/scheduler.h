//
//  scheduler.h
//  sh142
//
//  Created by Torkil Aamodt on 11/20/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef sh142_scheduler_h
#define sh142_scheduler_h

#include "definitions.h"
#include "jobs.h"
#include <time.h>

static pthread_t schedThread;
static int cpuLimit = 100, cpuTime = 100, memLimit = 100, memTime = 100;
static long totCpuTime = 0, cpuLimitJiffies = 0;
static long totMemTime = 0, memLimitJiffies = 0;
static long timeLimitSeconds = 250;
static bool isInitialized = false;

void *restrictProcesses(void* param);
void restrictProcesses(job j);
int checkOnProcess(job j);
long initScheduler(void);

int setTimeMax(char* cmd);
int setMemMax(char* cmd);
int setCpuMax(char* cmd);

#endif
