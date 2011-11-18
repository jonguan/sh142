//
//  jobs.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "jobs.h"

// Mini shell approach

/*
 void putJobInForeground(job* job, int cont)
 {
 job->status = FOREGROUND;
 tcsetpgrp(SHELL_TERMINAL, job->pgid);
 if (cont) {
 if (kill(-job->pgid, SIGCONT) < 0)
 perror("kill (SIGCONT)");
 }
 
 waitJob(job);
 tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
 }
 
 // Mini shell approach
 void putJobInBackground(job* job, int cont)
 {
 if (job == NULL)
 return;
 
 if (cont && job->status != WAITING_INPUT)
 job->status = WAITING_INPUT;
 if (cont)
 if (kill(-job->pgid, SIGCONT) < 0)
 perror("kill (SIGCONT)");
 
 tcsetpgrp(SHELL_TERMINAL, SHELL_PGID);
 }*/