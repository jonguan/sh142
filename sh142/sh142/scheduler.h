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

static int CPUPercentage, CPUSeconds;

int checkOnProcess(pid_t pid);

int setCpuMax(char* cmd);

#endif
