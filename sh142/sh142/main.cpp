//
//  main.c
//  sh142
//
//  Created by Øyvind Tangen on 25.10.11.
//  Copyright 2011 San Jose State University. All rights reserved.
//


#include "main.h"
#include "pipe.h"
#include "jobs.h"
#include "history.h"
//#include "forLoop.h"

void printPrompt() {
    char *pathPtr = getcwd(currentPath, 1024);
    printf("%s%s ", pathPtr, promptSignature);
}

void init() {
    // Initialize variables
    commandIdx = -1;
    command[0] = '\0';
    commandNumber = 0;
    
    currentPath = (char*) calloc(1024, sizeof(char));
    dataPath = (char*) calloc(1024, sizeof(char));
    execPath = (char*) calloc(1024, sizeof(char));
    promptSignature = (char*) calloc(16, sizeof(char));
    *promptSignature = '\0';
    
    readConfigFile();
    if (*promptSignature == '\0') promptSignature = (char*)"§";
    
    saveCommandToHistory((char*)"-- no more commands --");
    
    printPrompt();
    
    /*char *cmd[2];
    cmd[0] = (char*) "emacs";
    cmd[1] = (char*) NULL;
    //cmd[2] = NULL;
    
    launchJob(cmd, FOREGROUND, (char*)"DEFAULT", 0);
    int launchJob(char* cmd[], int mode, char* path, int flag);*/
}

#pragma mark - Configuration methods

void readConfigFile()
{
    configFile = fopen(".sh142", "r");
    if (configFile != NULL) {
        //printf("CONFIG FILE FOUND\n");
		char str1[128];
		char str2[128];
		int c1;
		int c2;
        int counter = 0;
		int first = 1;
        while(!feof(configFile)) {
            char c = getc(configFile);
			if (c == '\n') {
				c2 = counter;
				counter = 0;
				loadConfig(str1, c1, str2, c2);
				memset(str1, 0, sizeof(str1));
				memset(str2, 0, sizeof(str2));
				first = 1;
				counter = 0;
			}
			if (c == ' ') {
				first = 0;
				c1 = counter;
				counter = 0;
			}
			if (first == 1) {
				if (c != '\n' && c != ' ') {
					str1[counter] = c;
					counter++;
				}
			} else {
				if (c != '\n' && c != ' ') {
					str2[counter] = c;
					counter++;
				}
			}
        }
        
    }
	fclose(configFile);
}


void loadConfig(char str1[], int c1, char str2[], int c2) {
	char config[c1];
	char value[c2];
	int i;
	for (i = 0; i < c1; i++) {
		config[i] = str1[i];
	}
	
	for (i = 0; i < c2; i++) {
		value[i] = str2[i];
	}
	
	if (strcmp(config, "PromptSignature") == 0) {
		promptSignature = value;
	}
    else if (strcmp(config, "DataPath") == 0) {
        dataPath = value;
    }
    else if (strcmp(config, "ExecPath") == 0) {
        execPath = value;
    }
    /*else if (strcmp(config, "CmdLength") == 0) {
     CMD_LEN = atoi(value);
     }*/
}

int generateConfig() {
    configFile = fopen(".sh142", "w");
    int loop = 1, status;
    for (int i = 0; loop; i++) {
        switch (i) {
            case 0: status = putLnToFile(configFile, (char*)"DataPath", dataPath); break;
            case 1: status = putLnToFile(configFile, (char*)"ExecPath", execPath); break;
            case 2: status = putLnToFile(configFile, (char*)"PromptSignature", promptSignature); break;
                
            default:loop = 0; break;
        }
        if (status) {
            return 1;
        }
    }
    fclose(configFile);
    return 0;
}

int putLnToFile(FILE* dest, char* key, char* val) {
    if (fputs(key, dest) &&
        fputc(' ', dest) &&
        fputs(val, dest) &&
        fputc('\n', dest)) {
        return 0;
    }
    return 1;
}


#pragma mark - Main Method
/**
 main
 @param argc count of parameters passed in
 @param argv array of strings passed in
 @returns 0 if success; any other value if fail
 */
int main (int argc, const char * argv[])

{
    init();
    while (1) {
        //TODO: getKeyPress() doesn't work properly. I think.
        char c = /*fgetc(stdin);*/ getKeyPress();
        
        //printf("pressed key: %d\n", c);
        //continue;
        
        if (c == 127) { //Special case: Backspace
            command[commandIdx--] = '\0';
            continue;
        }
        //Yup, -17, -100 and -128/-127 all together makes the arrow up/down-button
        else if (c == -17 && getKeyPress() == -100) {
            c = getKeyPress();
            if (c == -128) { //Arrow Up hit
                resetCommandBuffer();
                loadPreviousCommandFromHistory(command, &commandIdx);
            } else if (c == -127) { //Arrow Down hit
                resetCommandBuffer();
                loadNextCommandFromHistory(command, &commandIdx);
            }
            continue;
        } else if (++commandIdx >= CMD_LEN) { //Special case: buffer is full
            perror((char*)"Command buffer is full.");
            resetCommandBuffer();
            printPrompt();
            continue;
        }
        
        command[commandIdx] = c; //Assigns a char from keyboard input to command string
        
        //User hit return - replace \n with \0
        if (command[commandIdx] == '\n') { //The whole command is stored in command array
            command[commandIdx] = '\0';
            saveCommandToHistory(command);
            
            //Parser goes here 
            if (EXIT == parseInput(command)) {
                return 0;
            }
            
            resetCommandBuffer();
            printPrompt();
        }
    }
    return EXIT_SUCCESS;
}

//TODO: This is basically ripped of teh internetz, needs rewriting.
char getKeyPress() {
    struct termios t, newT;
    char c;
    
    tcgetattr(STDIN_FILENO, &t);
    newT = t;
    newT.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newT);
    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    
    return c;
}

void resetCommandBuffer() {
    for (char* c = command; *c != '\0'; c++) *c = '\0';
    
    /*for (int i = commandIdx; i >= 0; i--) {
        command[i] = '\0';
        //printf("%c", '\b');
        //printf("/b");
        //printf( "%c[2J", 27 );
    }*/
    //int c;
    //while ((c = getchar()) != '\n' && c != EOF);
    //fpurge(stdin);
    commandIdx = -1;
}

#pragma mark - Parser
int parseInput(char *inputCommand)
{
    int numForLoops = 0;
    int returnValue = UNINITIALIZED;
    char *subCommand = inputCommand; //pointer to beginning of subCommand
    char* c = inputCommand;    // iterator through cmd
    int AndExpression = UNINITIALIZED;
    
    // Break up cmd into subCommands by checking for special characters
    // for (c = inputCommand; *c != '\0'; c++) {
    while (*c != '\0') {
        // Top level checks for logical operators && and || and for loops
     
         if (*c == '&' && !strncmp(c, "&&", 2) && numForLoops == 0) {
            *c = '\0';
            if (returnValue == EXIT_FAILURE) {
                // Fast returning
                rememberExitStatus(returnValue);
                return returnValue;
            }
            AndExpression = TRUE;
            returnValue = returnValue == UNINITIALIZED ? cmdInterpreter(subCommand) : returnValue || cmdInterpreter(subCommand);
            c+=2;
            subCommand = c;
        }
        else if(*c == '|' && !strncmp(c, "||", 2) && numForLoops == 0){
            *c = '\0';
            if (returnValue == EXIT_SUCCESS) {
                // Fast return
                rememberExitStatus(returnValue);
                return returnValue;
            }
            AndExpression = FALSE;
            returnValue = returnValue == UNINITIALIZED ? cmdInterpreter(subCommand) : returnValue && cmdInterpreter(subCommand);
            c+=2;
            subCommand = c;
        }
        else if (*c == 'f'){
            //Increment or decrement forloop counter
            if(!strncmp("forend", c, 6)) {
                c+=6;
                numForLoops--;
            }else if(!strncmp("for ", c, 4) || !strncmp("for(", c, 4)){
                c+=4;
                numForLoops++;
            }

        }else{
            c++;
        }
        
        if (returnValue == EXIT) {
            return returnValue;
        } 
      
    }
    
    // end of command - send all prevous to cmdInterpreter
    if (returnValue == UNINITIALIZED) {
        returnValue = cmdInterpreter(subCommand);
    }else{
        returnValue = AndExpression ? returnValue || cmdInterpreter(subCommand) : returnValue && cmdInterpreter(subCommand);                
    }
    
       
    rememberExitStatus(returnValue);
 
    return returnValue;
}


int rememberExitStatus(int exitStatus)
{
    //Put returnValue into the exit Status Array
    exitStatusArray[commandNumber % NUM_REMEMBERED_CMDS] = exitStatus == UNINITIALIZED ? 0 : exitStatus;
    commandNumber ++;

    return EXIT_SUCCESS;
}

void printExitStatus(){
    // Print returnValues for debug
    
     for (int i = 0; i < (commandNumber % NUM_REMEMBERED_CMDS); i++) {
     printf("%d\n", exitStatusArray[i]);
     }
     printf("\n");
     
}

/**
	parsePipeCommand - checks if subCommand has any pipes or for loops in it, and calls functions accordingly
 
    NOTE: we can't send everything to bash shell immediately because of special characters $?1 and for loop
 
	@param command - subcommand from parseInput
	@returns process return value
 */
int parsePipeCommand(char *command)

{
    int returnValue = UNINITIALIZED;
  
/*
    char *restOfCommand = strstr(command, "|");
    if (restOfCommand != NULL) {
        //Is a pipe command!
        size_t lenFirstCmd = restOfCommand-command;
        char firstCommand[lenFirstCmd];
        strncpy(firstCommand, command, lenFirstCmd);
        firstCommand[lenFirstCmd] = '\0';
        restOfCommand++;
        returnValue = runPipeCommand(firstCommand, restOfCommand);
  */      
    char *pipeStart = strstr(command, "|");
    if(pipeStart != NULL){
        // is pipe command
        char *subcommand = strtok(command, "|");
        size_t nbytes = SIZE_PIPE_BUFFER;
        char *data = (char *) malloc (nbytes + 1);
        
        while (subcommand != NULL) {
            //send individual commands to cmdinternal
            if (returnValue == UNINITIALIZED) {
                //need to pass through cmdInterpreter to catch any commands not applicable to bash
                returnValue = runPipeReadCommand(subcommand, data);
                //printf("%s", data);
            }else{
                char *nextData = (char *) malloc (nbytes + 1);
                returnValue = runPipeWriteCommand(subcommand, data, nextData);
                //free(data);
                data = nextData;
            }

            subcommand = strtok(NULL, "|");
        } 
        printf("%s\n", data);
         
        
        return returnValue;
    }else{
        // is a regular command
        returnValue = cmdInterpreter(command);
        
    }
 
    return returnValue;
    
}

// Gets entire beginning of for loop
// if complete syntax is not given, then will print > for more input
// returns exit status; upon return, forLoop will be set to the command input after forend, or NULL if finished
int runForLoopParser(char *forLoop)
{
    char *c = forLoop;
    
    //assume for loop is given in entirety
    if (!strncmp(forLoop, "for", 3)) {
        c+=3;
        
    }
    //size_t forLength = strlen(forLoop);
    while (1) {
        char c = /*fgetc(stdin);*/ getKeyPress();
        
        
        if (c == 127) { //Special case: Backspace
            command[commandIdx--] = '\0';
            continue;
        }
        else if (++commandIdx >= CMD_LEN) { //Special case: buffer is full
            perror((char*)"Command buffer is full.");
            resetCommandBuffer();
            printPrompt();
            continue;
        }
        
        command[commandIdx] = c; //Assigns a char from keyboard input to command string
        
        //User hit return - replace \n with \0
        if (command[commandIdx] == '\n') { //The whole command is stored in command array
            command[commandIdx] = '\0';
            
            //Parser goes here 
            if (EXIT == parseInput(command)) {
                return EXIT_SUCCESS;
            }
            
            resetCommandBuffer();
            printPrompt();
        }
    }
    return EXIT_SUCCESS;
}

#pragma mark - Command Interpreter methods
int getPastReturnValueAtIndex(int index){
    if (index > NUM_REMEMBERED_CMDS){
        printf("Number of commands remembered is %d\n", NUM_REMEMBERED_CMDS);
    }else{
        int arraySpot = (index < commandNumber) ? (commandNumber - index) : (NUM_REMEMBERED_CMDS + commandNumber - index -1);
        printf("%d\n", exitStatusArray[arraySpot]);      
        return EXIT_SUCCESS; //exitStatusArray[arraySpot];
    }
    return EXIT_FAILURE;
}


/*
 Checks for internal commands first, then external (from data path) later.
 return value of 0 if successful
 */
int cmdInterpreter (char* cmd) {
    int returnValue = UNINITIALIZED;
    char *subcommand = cmd;
    char* c = cmd;    // iterator through cmd
    
    //Set up storage buffer
    char *data = (char *) malloc (SIZE_PIPE_BUFFER + 1); //buffer for pipe
   
    
    while (*c != '\0') {
        
        
        if(!strncmp("for ", c, 4) || !strncmp("for(", c, 4)){
            //for loop - send 
            //returnValue = runForLoopParser(c);
            if (c == NULL) {
                //finished
                break;
            }
        }
        else if (*c == '|'){
            //Pipe command
            *c = '\0';
            
            if (returnValue == UNINITIALIZED) {
                // run command, put in data
                returnValue = runPipeReadCommand(subcommand, data);
            }else{
                // send data in and store into buffer
                char *nextData = (char *) malloc (SIZE_PIPE_BUFFER + 1);
                returnValue = runPipeWriteCommand(subcommand, data, nextData);
                free(data);
                data = nextData;
            }
            
            // get next process to pass results
            c++;
            subcommand = c;
        }
        else if (*c == '=' && isalpha(*subcommand)){
            //Environment variable
            *c = '\0';
            c++;
            //get operand 
            char *restOfString = strcpy(restOfString, c);
            char *operand = strtok(restOfString, " ;");
            setEnvironmentVariable(subcommand, operand);
            
            c+= strlen(operand);
            subcommand = c;
        }else{
            c++;
        }
        
    }

    
    if (returnValue == UNINITIALIZED) {
        returnValue = cmdInterpreterInternal(cmd);  
        if (returnValue == EXIT) 
            return EXIT; //Special case: exit
        else if (returnValue != EXIT_FAILURE) 
            rememberExitStatus(returnValue); //Command was handled internally
        else if (!cmdInterpreterExternal(cmd))
            rememberExitStatus(EXIT_SUCCESS); //Command was handled externally
        else { //Command was not recognized
            printf("Unknown command: '%s'\n", cmd);
            rememberExitStatus(EXIT_FAILURE);
            return EXIT_FAILURE;
        }

    }
        
        
    free(data);
    return EXIT_SUCCESS;
}

/*
 Attempts to apply a command internally.
 (Add new internal commands in this method.)
 
 cmd points to first char in command.
 mid points to the first char after the command name.
 end points to the last char of the command.
 
 Returns 0 if command was processed, else 1.
 */
int cmdInterpreterInternal (char* cmd) {
    if (cmd == '\0') {
    } 
    else if (!strncmp("exitStatus", cmd, 10)){
        printExitStatus();
    }
    else if (!strncmp(cmd, "exit", 4)) {
        return -1;
    } 
    else if (!strncmp(cmd, "jobs", 4)){
        listJobs();
    }
    else if (!strncmp(cmd, "kill", 4)){
        
    }
    else if (!strncmp("$?", cmd, 2)) {
        // exit status history
        char *c = cmd;
        c += 2;
        if (isdigit(*c)){
            char *space = strstr(c, " ");
            if (space != NULL) {
                *space = '\0';
                c = space+1;
                
            }
            
            int index = atoi(c);
            return getPastReturnValueAtIndex(index);
            
        }else{
            // have $%notANumber
            return EXIT_FAILURE;
        }
    }
    else return EXIT_FAILURE; //not an internal command
    return EXIT_SUCCESS; //Command was processed as internal
}

/**
	cmdInterpreterExternal
    TODO: Parse out arguments from - and insert each cmd into jobs linked list
	@param cmd - individual commands free from symbols except for " " and -
	@param end 
	@returns process return value
 */
int cmdInterpreterExternal (char* cmd) {

    
    /*
     char *ptr;
     char *startPtr = execPath;
     for (ptr = execPath; *ptr != '\0'; ptr++) {
     if (*ptr == ':') {
     *ptr = '\0';
     printf("%s\n", startPtr); //TODO: Match cmd to path contents
     startPtr = ptr + 1;
     *ptr = ':';
     }
     }
     printf("%s\n", startPtr);
     return 1;
     */
    
    int mode = UNINITIALIZED;
    char* ampersand = strstr(cmd, "&");
    if (ampersand - cmd > 0) {
        mode = BACKGROUND;
    }else{
        mode = FOREGROUND; 
    }
        
    char *token = strtok(cmd, " ");
    char *tokens[20];
    int i = 0;
    while (token != NULL) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    tokens[i] = '\0';
    return launchJob(tokens, mode, (char*)"DEFAULT", 0);
    
}


#pragma mark - Environment Variables
int setEnvironmentVariable(char* variable, char*operand){
    if (!strncmp(variable, "PATH", 4)) {
        setExecPath(operand);
    } 
    else if (!strncmp(variable, "DATA", 4)) {
        setDataPath(operand);
    }else if(*variable >= 'A' && *variable <= 'Z'){
        //Capital letter only
        // insert into linked list?
        
        // need to write a getter and a setter, and also how to delete
        
    }

    return EXIT_SUCCESS;
}

int setExecPath(char* cmd) {
    if (!setPath(cmd, execPath)) {
        printf("Executable path set as '%s'\n", execPath);
        return 0;
    } else {
        perror((char*)"Executable path was not set.");
        return 1;
    }
}

int setDataPath(char* cmd) {
    if (!setPath(cmd, dataPath)) {
        printf("Data path set as '%s'\n", dataPath);
        return 0;
    } else {
        perror((char*)"Datapath was not set.");
        return 1;
    }
}

int setPath(char* cmd, char* p) {
    if (strlen(cmd) > 1024 || validatePaths(cmd)) return 1;
    
    /*
    char* pathPtr = p;
    for (char* c = cmd; c != end; c++) {
        *pathPtr = *c;
        pathPtr++;
    }
    *pathPtr = '\0';*/
    generateConfig();
    return 0;
}

int validatePaths(char* pathList) {
    int foundErrors = 0;
    char* ptr;
    char* ptrStart = pathList;
    struct stat s;
    for (ptr = pathList; *ptr != '\0'; ptr++) {
        if (*ptr == ':') {
            *ptr = '\0'; //Temporarily cuts array short
            if(stat(ptrStart, &s)) {
                printf("Error: Path '%s' does not exist.\n", ptrStart);
                foundErrors++;
            }
            *ptr = ':';
            ptrStart = ptr + 1;
        }
    }
    if(stat(ptrStart, &s)) {
        printf("Error: Path '%s' does not exist.\n", ptrStart);
        foundErrors++;
    }
    return foundErrors;
}

