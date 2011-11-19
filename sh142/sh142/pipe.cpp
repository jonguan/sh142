//
//  pipe.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 San Jose State University. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "pipe.h"


//Command is full command input of from the buffer
int runExternalCommand(char *command)
{
    char *cmd = strtok(command, " ");
    
    return runSubCommand(cmd);
    
}

int runPipeCommand(char** inCommand, char** outCommand)
{
    pid_t pid;
    int fds[2];
    
    // Create pipe for all processes
    if (pipe(fds)) {
        fprintf(stderr, "Pipe failed.\n");
        return EXIT_FAILURE;
    }

    /* create child processes*/
    pid = fork();
    

    if (pid == 0) {
    //child process 1
        
        // reassign stdin to fds[0]
        dup2(fds[0], STDIN_FILENO); //Set 0 to input
        close(fds[1]);
        close(fds[0]);
        
        //another child process
        if (fork() == 0) {
            dup2(fds[1], STDOUT_FILENO); //set 1 to stdout
            close(fds[0]);
            close(fds[1]);
            
            //execute 1st command
            //execvp(inCommand, inCommand);
        }
     
        wait(NULL);
        //execvp(outCommand, outCommand);
        
                
    }
    
    close(fds[1]);
    close(fds[0]);
    wait(NULL);
    
    return 0;
    
}
int runPipeReadCommand(char *command, char*result)
{
    FILE *read_pipe;
   
    long bytes_read;
    size_t nbytes = 100;
    char *my_string;
    
    /* Open our two pipes */
    read_pipe = popen (command, "r");
       
    /* Check that pipes are non-null, therefore open */
    if (!read_pipe)
    {
        fprintf (stderr,
                 "Failed to create pipe with command %s.\n", command);
        return EXIT_FAILURE;
    }
    
    /* Read from ps_pipe until two newlines */
    my_string = (char *) malloc (nbytes + 1);
    bytes_read = getdelim (&my_string, &nbytes, '\0', read_pipe);
    
    /* Close ps_pipe, checking for errors */
    if (pclose (read_pipe) != 0)
    {
        fprintf (stderr, "Could not run %s.\n", command);
        return EXIT_FAILURE;
    }
            
    /* Send output of 'ps -A' to 'grep init', with two newlines */
    
    return 0;
}


int runPipeWriteCommand(char *command, char* inputString, char* result)
{
    
    FILE *out_pipe;
    
    long bytes_read;
    size_t nbytes = 100;

    /* Open our two pipes */
   // ps_pipe = popen (command, "r");
    out_pipe = popen (command, "w");
    // header_pipe = popen ("grep UID", "w");
    
    /* Check that pipes are non-null, therefore open */
    if (!out_pipe)
    {
        fprintf (stderr,
                 "Failed to open pipe with command %s.\n", command);
        return EXIT_FAILURE;
    }
    
    /* Send output of 'ps -A' to 'grep init', with two newlines */
    fprintf (out_pipe, "%s\n\n", inputString);
    

    // Try to read out_pipe into a string
    
    

    /* Close grep_pipe, cehcking for errors */
    
    if (pclose (out_pipe) !=0){
        fprintf (stderr, "could not run %s, or other error\n", command);
        return EXIT_FAILURE;
    }
    
    
    bytes_read = getdelim (&result, &nbytes, '\0', out_pipe);  
    
    /* Exit! */
    
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
