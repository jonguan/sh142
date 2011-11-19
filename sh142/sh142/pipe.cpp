//
//  pipe.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "pipe.h"

//Command is full command input of from the buffer
int runExternalCommand(char *command)
{
    char *cmd = strtok(command, " ");
    
    return runSubCommand(cmd);
    
}

int runPipeCommand(char *command)
{
    return 0;
}

int runSubCommand(char *subCommand){
    FILE *ps_pipe;
    //FILE *grep_pipe;
    //FILE *header_pipe;
    
    long bytes_read;
    size_t nbytes = 100;
    char *my_string;
    
    /* Open our two pipes */
    ps_pipe = popen (subCommand, "r");
    // grep_pipe = popen ("grep 0", "w");
    // header_pipe = popen ("grep UID", "w");
    
    /* Check that pipes are non-null, therefore open */
    if ((!ps_pipe) /*|| (!grep_pipe) || (!header_pipe)*/)
    {
        fprintf (stderr,
                 "One or both pipes failed.\n");
        return EXIT_FAILURE;
    }
    
    /* Read from ps_pipe until two newlines */
    my_string = (char *) malloc (nbytes + 1);
    bytes_read = getdelim (&my_string, &nbytes, '\0', ps_pipe);
    
    /* Close ps_pipe, checking for errors */
    if (pclose (ps_pipe) != 0)
    {
        fprintf (stderr,
                 "Could not run %s.\n", subCommand);
    }
    //fprintf (header_pipe, "%s\n", my_string);
    /* Send output of 'ps -A' to 'grep init', with two newlines */
    //fprintf (grep_pipe, "%s\n\n", my_string);
    printf("%s\n\n", my_string);
    
    /* Close grep_pipe, cehcking for errors */
    /*
     if (pclose (header_pipe) !=0)
     fprintf (stderr, "could not run 'grep UID', or other error\n");
     
     if (pclose (grep_pipe) != 0)
     {
     fprintf (stderr,
     "Could not run 'grep 0', or other error.\n");
     }*/
    
    /* Exit! */
    free(my_string);
    return 0;
    
}
