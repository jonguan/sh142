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
#include "jobs.h"

int runPipeParser(char *pipeCommands){
    int returnValue = UNINITIALIZED;
/*
    char *allCommands[25];
    int numcommands = 0;
    char *command = strtok(pipeCommands, "|");
    while (command != NULL) {
        //insert into array
        allCommands[numcommands] = command;
        command = strtok(NULL, "|");
        numcommands++;
    }
    allCommands[numcommands] = '\0';
  */
    
    char *c = pipeCommands;
    char *subcommand = pipeCommands;
    //Set up storage buffer
    char *data = (char *) malloc (SIZE_PIPE_BUFFER + 1); //buffer for pipe
    //char *data = "file";
    
    while (*c != '\0') {
        if (*c == '|') {
            *c = '\0';
            
            if (returnValue == UNINITIALIZED) {
                // run command, put in data
                returnValue = runPipeReadCommand(subcommand, data);
             
            }else{
                // send data in and store into buffer
                char *nextData = (char *) malloc (SIZE_PIPE_BUFFER + 1);
                //char *nextData = "fileOut";
                returnValue = runPipeWriteCommand(subcommand, data, nextData);
                //copyFromFile(data, nextData);
                free(data);
                data = nextData;
            }
            
            c++;
            subcommand = c;
        }else{
            c++;
        }
        
        
    }

    if (returnValue == UNINITIALIZED) {
        // run command, put in data
        returnValue = runPipeReadCommand(subcommand, data);
      
    }else{
        // send data in and store into buffer
        char *nextData = (char *) malloc (SIZE_PIPE_BUFFER + 1);
        //char *nextData = "fileOut";
        returnValue = runPipeWriteCommand(subcommand, data, nextData);
        
        free(data);
        data = nextData;
    }
    free(data);
    return returnValue;
}

int copyFromFile(char*destination, char*source){
    FILE *in, *out;
    char ch;
    
    if((in=fopen(source, "r")) == NULL) {
        printf("Cannot open input file.\n");
        exit(1);
    }
    if((out=fopen(destination, "w")) == NULL) {
        printf("Cannot open output file.\n");
        exit(1);
    }
    
    while(!feof(in)) {
        ch = getc(in);
        if(ferror(in)) {
            printf("Read Error");
            clearerr(in);
            break;
        } else {
            if(!feof(in)) putc(ch, out);
            if(ferror(out)) {
                printf("Write Error");
                clearerr(out);
                break;
            }
        }
    }
    fclose(in);
    fclose(out);
    return EXIT_SUCCESS;
}

//Command is full command input of from the buffer
int runExternalCommand(char *command)
{
    char *cmd = strtok(command, " ");
    return runSubCommand(cmd);
    
}

/************************
 function: void pipeCommand(char** cmd1, char** cmd2)
 comment: This pipes the output of cmd1 into cmd2.
 **************************/
int pipeCommand(char *cmd1[], char*cmd2[]) {
    const int PIPE_READ = 0;
    const int PIPE_WRITE = 1;
    int fds[2]; // file descriptors
    pipe(fds); // create the pipe
    
    // Execute the second command.
    // child process #2
    if (fork() == 0) {
        // Reassign stdout to fds[1] end of pipe.
        dup2(fds[PIPE_WRITE], STDOUT_FILENO);
        close(fds[PIPE_READ]);
        close(fds[PIPE_WRITE]);
        // Execute the first command.
        execlp(*cmd1, *cmd1, NULL);
        
        /*
         if (execvp(*cmd1, cmd1) == -1){
         return EXIT_FAILURE;
         }*/
        //return EXIT_SUCCESS;
    }
    
    // child process #1
    if (fork() == 0) {
        // Reassign stdin to fds[0] end of pipe.
        dup2(fds[PIPE_READ], STDIN_FILENO);
        close(fds[PIPE_WRITE]);
        close(fds[PIPE_READ]);
             //   wait(NULL);
        execlp(*cmd2, *cmd2, NULL);
        //
      
        /*
        if (execvp(*cmd2, cmd2) == -1){
            return EXIT_FAILURE;
        }*/
        //return EXIT_SUCCESS;
    }
    

    close(fds[PIPE_WRITE]);
    close(fds[PIPE_READ]);
    wait(NULL);
    wait(NULL);
    return EXIT_SUCCESS;
}

/*
int runPipeCommand(char* inCommand, char *outCommand)
{
    FILE *in_pipe;
    FILE *out_pipe;
   // FILE *out_read_pipe;
    
    long bytes_read;
    size_t nbytes = SIZE_PIPE_BUFFER;
    char *my_string;
    
    //Open our two pipes 
    in_pipe = popen (inCommand, "r");
    out_pipe = popen (outCommand, "w");
   // out_read_pipe = popen(, "r");
    
    // Check that pipes are non-null, therefore open 
    if ((!in_pipe) || (!out_pipe))
    {
        fprintf (stderr,
                 "One or both pipes failed.\n");
        return EXIT_FAILURE;
    }
    
    //Read from ps_pipe until two newlines 
    my_string = (char *) malloc (nbytes + 1);
    bytes_read = getdelim (&my_string, &nbytes, '\0', in_pipe);
    
    // Close ps_pipe, checking for errors 
    if (pclose (in_pipe) != 0)
    {
        fprintf (stderr,
                 "Could not run %s.\n", inCommand);
    }

    
    // Send output of 'ps -A' to 'grep init', with two newlines 
    fprintf (out_pipe, "%s\n\n", my_string);
    //printf("%s\n\n", my_string);
    
    // Close grep_pipe, cehcking for errors 
    if (pclose (out_pipe) != 0)
    {
        fprintf (stderr,
                 "Could not run %s, or other error.\n", outCommand);
    }
    
    //Exit! 
    free(my_string);
    return 0;

}*/

/*
int runPipeCommand(char** inCommand, char** outCommand)
{
    pid_t pid;
    int fds[2];
    
    // Create pipe for all processes
    if (pipe(fds)) {
        fprintf(stderr, "Pipe failed.\n");
        return EXIT_FAILURE;
    }

    // create child processes
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
    
}*/
/*
int runPipeReadCommand(char *command, char*file)
{
    int redirectType = STDOUT;
    int mode = FOREGROUND;
    
    //tokennize the command
    char *token = strtok(command, " ");
    char *tokens[10];
    int i = 0;
    while (token != NULL) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, " &");
    }
    tokens[i] = '\0';

    
    return launchJob(tokens, file , redirectType, mode);

}

int runPipeWriteCommand(char *command, char* fileIn, char* fileOut){
    int redirectType = STDIN;
    int mode = FOREGROUND;
    
    //tokennize the command
    char *token = strtok(command, " ");
    char *tokens[10];
    int i = 0;
    while (token != NULL) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, " &");
    }
    tokens[i] = '\0';
    
    

     launchJob(tokens, fileIn , redirectType, mode);
    //launchPipeJob(tokens, fileIn, fileOut, mode);
}*/


int runPipeReadCommand(char *command, char*file)
{
    FILE *read_pipe;
   
    long bytes_read;
    size_t nbytes = SIZE_PIPE_BUFFER;
    //char *my_string;
    
    // Open our two pipes 
    read_pipe = popen (command, "r");
       
    //Check that pipes are non-null, therefore open 
    if (!read_pipe)
    {
        fprintf (stderr,
                 "Failed to create pipe with command %s.\n", command);
        return EXIT_FAILURE;
    }
    
    // Read from ps_pipe until two newlines 
    //my_string = (char *) malloc (nbytes + 1);
    bytes_read = getdelim (&file, &nbytes, '\0', read_pipe);
    
    // Close ps_pipe, checking for errors 
    if (pclose (read_pipe) == -1)
    {
      //  fprintf (stderr, "Could not run %s.\n", command);
        return EXIT_FAILURE;
    }
            
    //Send output of 'ps -A' to 'grep init', with two newlines 
   // printf("%s\n", result);
    return 0;
}


int runPipeWriteCommand(char *command, char* fileIn, char* fileOut)
{
    
    FILE *out_pipe;
    
    long bytes_read;
    size_t nbytes = SIZE_PIPE_BUFFER;

    
   // ps_pipe = popen (command, "r");
    out_pipe = popen (command, "w");
    // header_pipe = popen ("grep UID", "w");
    
    // Check that pipes are non-null, therefore open 
    if (!out_pipe)
    {
        fprintf (stderr,
                 "Failed to open pipe with command %s.\n", command);
        return EXIT_FAILURE;
    }
    
    // Send output of 'ps -A' to 'grep init', with two newlines 
   // printf("%s", inputString);
    fprintf (out_pipe, "%s", fileIn);
    

    // Try to read out_pipe into a string
    
    

    // Close grep_pipe, cehcking for errors 
    
    if (pclose (out_pipe) !=0){
       // fprintf (stderr, "could not run %s, or other error\n", command);
        return EXIT_FAILURE;
    }
    
    
    bytes_read = getdelim (&fileOut, &nbytes, '\0', out_pipe);  
    //printf("%s", result );
    
    // Exit! 
    
    return 0;

}

int runSubCommand(char *subCommand){
    FILE *ps_pipe;
    //FILE *grep_pipe;
    //FILE *header_pipe;
    
    long bytes_read;
    size_t nbytes = SIZE_PIPE_BUFFER;
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
    if (pclose (ps_pipe) == -1)
    {
        fprintf (stderr,
                 "Could not run %s.\n", subCommand);
        return EXIT_FAILURE;
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

int redirectToFile (char *command){
    int returnValue = UNINITIALIZED;
    char *cmdPtr = command;
    char *processName = command;
    char filename[30];
    
    while (*cmdPtr != '\0') {
        if (*cmdPtr == '>' || *cmdPtr == '<' || (*cmdPtr == '2' && !strncmp(cmdPtr, "2>", 2))) {
            int redirectType = -1;
            switch (*cmdPtr) {
                case '<':
                    redirectType = STDIN;
                    break;
                case '>':
                    redirectType = STDOUT;
                    break;
                case '2':{
                    cmdPtr++; //for the 2>
                    redirectType = STDERR;
                }
                    break;
                default:
                    break;
            }
            
            *cmdPtr = '\0';
            cmdPtr++;
            //remove whitespace
            while (*cmdPtr == ' ') {
                cmdPtr++;
            }
            strcpy(filename, cmdPtr);
            //filename = cmdPtr;
            //stdout to file
            //freopen(filename, "w", stdout);
            //cmdInterpreter(processName);
            int mode = FOREGROUND;
            char *token = strtok(processName, " ");
            char *tokens[20];
            int i = 0;
            while (token != NULL) {
                tokens[i] = token;
                i++;
                token = strtok(NULL, " &");
            }
            tokens[i] = '\0';
            
            
            
            /*
             launchJob
             @param cmd - array of strings from command where cmd[0] = command name and all else are descriptors
             @param pa
             th - path to a file
             @param flag - STDIN or STDOUT
             @param mode - FOREGROUND or BACKGROUND
             */
        
            returnValue = launchJob(tokens, filename , redirectType, mode);
        }else{
            cmdPtr++;
        }
    }
  
    return returnValue;
}
