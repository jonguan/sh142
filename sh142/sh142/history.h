//
//  history.h
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef sh142_history_h
#define sh142_history_h

#include "definitions.h"

static char cmdHistory[CMD_HISTORY_LEN][CMD_LEN];
static int historyIdx; //Points to where in history to put next entered command
static int historyViewIdx; //Points to which command in history to view next when arrow-keys are pressed

int loadPreviousCommandFromHistory();
int loadNextCommandFromHistory();
int loadCommandFromHistory(int idx);


#endif
