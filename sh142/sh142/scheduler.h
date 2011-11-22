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
static int cpuLimit = 100, cpuTime = 100;
static long cpuLimitJiffies = 0;

void *restrictProcesses(void* param);
void restrictProcesses(job j);
int checkOnProcess(job j);

int setCpuMax(char* cmd);

#endif
