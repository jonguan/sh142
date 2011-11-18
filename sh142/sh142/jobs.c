//
//  job.c
//  sh142
//
//  Created by Øyvind Tangen on 17.11.11.
//  Copyright 2011 San Jose State Univeristy. All rights reserved.
//

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