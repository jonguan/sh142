//
//  main.c
//  sh142
//
//  Created by Øyvind Tangen on 25.10.11.
//  Copyright 2011 San Jose State University. All rights reserved.
//


#include "main.h"

//#include "uthash.h"

#pragma mark - Hash Table

struct envVar {
    char name[15];             /* we'll use this field as the key */
    size_t value;
    UT_hash_handle hh; /* makes this structure hashable */
};

static struct envVar *sh142Vars = NULL;

void add_var(struct envVar *var) 
{
    HASH_ADD_STR(sh142Vars, name, var);
}

struct envVar *find_var(char* varName) 
{
    struct envVar *varPtr = NULL;
    HASH_FIND_STR(sh142Vars, varName, varPtr);
    return varPtr;
}


void delete_var(struct envVar *var) 
{
    HASH_DEL( sh142Vars, var);  
}



#pragma mark - main
void printPrompt() {
    char *pathPtr = getcwd(currentPath, 1024);
    printf("%s%s ", pathPtr, promptSignature);
}


void tester() {
    char *cmd[3];
    cmd[0] = (char*) "emacs";
    //cmd[1] = (char*) "&";
    cmd[1] = NULL;
    
    launchJob(cmd, (char*)"DEFAULT", 1, BACKGROUND);
    //listJobs();
    //killJob(1);
    //listJobs();
}

void init() {
    shellInit();
    
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
    
    //tester();
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
        char c = getchar();//fgetc(stdin); //getKeyPress();
        
        //printf("pressed key: %d\n", c);
        //continue;
        
        if (c == 127) { //Special case: Backspace
            command[commandIdx--] = '\0';
            continue;
        }
        //Yup, -17, -100 and -128/-127 all together makes the arrow up/down-button
        /*else if (c == -17 && getKeyPress() == -100) {
            c = getKeyPress();
            if (c == -128) { //Arrow Up hit
                resetCommandBuffer();
                loadPreviousCommandFromHistory(command, &commandIdx);
            } else if (c == -127) { //Arrow Down hit
                resetCommandBuffer();
                loadNextCommandFromHistory(command, &commandIdx);
            }
            continue;
        }*/ else if (++commandIdx >= CMD_LEN) { //Special case: buffer is full
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
        }else if (*c == '(' && ! strncmp(c, "((", 2)){
            //run evaluation on the command
            c+=2;
            char expression[20];
            char *endOfExp = strtok(expression, "))");
            size_t lengthExp = strlen(endOfExp);
            strncpy(expression, c, lengthExp);
            c+=(lengthExp +2);
            subCommand = c;
            returnValue = evaluateEnvVariable(expression);
        }
        else if (*c == 'f'){
            //Increment or decrement forloop counter
            if(!strncmp("forend", c, 6)) {
                c+=6;
                numForLoops--;
            }else if(!strncmp("for ", c, 4) || !strncmp("for(", c, 4)){
                c+=4;
                numForLoops++;
            }else{
                c++;
            }

        }
        else{
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
    int returnValue = UNINITIALIZED;
    int numOpenBrace = 0;
    char *loopPtr = forLoop;
    
    //assume for loop is given in entirety
    while (*loopPtr != '\0') {
        if (!strncmp(loopPtr, "forend", 6)) {
            //done
            loopPtr+=6;
            forLoop = loopPtr;
            return EXIT_SUCCESS;
        }  
        // nested for loop
        else if(!strncmp(loopPtr, "for ", 4) || !strncmp(loopPtr, "for(", 4)){
            returnValue = runForLoopParser(loopPtr);
            
        }
        else if(*loopPtr == '('){
            // set operands
            numOpenBrace++;
            char *restOfString = strcpy(restOfString, loopPtr);
            
            char *token = strtok(restOfString, ";");
            
            if (token == NULL) {
                //print command prompt for more input
            }else if (strstr(token, "=") == NULL){
                // ERROR - first command needs to have =
                *forLoop = '\0';
                return EXIT_FAILURE;
            }else{
                // give to cmd to evaluate
                cmdInterpreter(token);
            }
            
            //Handle evaluation of middle
            char *middle = strtok(NULL, ";");
            char middleEvalExp[30];
            sprintf(middleEvalExp, "((%s))", middle);
            
            // Handle evaluation of end
            char *end = strtok(NULL, ")");
            char endEvalExp[20];
            sprintf(endEvalExp, "((%s))", end);
            
            //Evaluate operand when finished with loop
            
            if (restOfString != NULL) {
                // Still evaluation left
                char *process = strtok(NULL, ";");
                
                // Store commands in an array
                char *processArray[10]; 
                int i = 0;
                while (process != NULL && strstr(process, "forend") != NULL) {
                    processArray[i] = process;
                    i++;
                    process = strtok(NULL, ";");
                }
                
                int lengthProcessArray = sizeof(processArray) / sizeof(processArray[0]);
                
                // Loop through processArray until end condition met
                while (!parseInput(middleEvalExp)) {
                    for (int j = 0; j < lengthProcessArray; j++) {
                        if (returnValue == UNINITIALIZED || returnValue == EXIT_SUCCESS) {
                            returnValue = parseInput(processArray[j]);
                        }else{
                            return returnValue;
                        }
                    }
                    parseInput(endEvalExp);
                }
                
            }
        }
        
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
            returnValue = runSubCommand(c);
            //returnValue = runForLoopParser(c);
            if (c == NULL) {
                //finished
                break;
            }
        }
        else 
            if (*c == '|'){
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
        } else if (*c == '=' && isalpha(*subcommand)){
            //Environment variable
            *c = '\0';
            c++;
            //get operand 
            char restOfString[40];
            strcpy(restOfString, c);
            char *operand = strtok(restOfString, " ;");
            returnValue = setEnvironmentVariable(subcommand, operand);
            
            c+= strlen(operand);
            subcommand = c;
        } else if (*c == '$' && strncmp("$?", c, 2)){
            //testing purposes here
            //get operand 
            c++;
            char restOfString[40];
            strcpy(restOfString, c);
            char *operand = strtok(restOfString, " ;");

            struct envVar *var = find_var(operand);

            
            if (var != NULL) {
                printf("%lu", var->value);                
            }else{
                printf("%s not found", operand);
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
        else{
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
    if (*cmd == '\0') {
    } 
    else if (!strncmp("exitStatus", cmd, 10)){
        printExitStatus();
    }
    else if (!strncmp(cmd, "exit", 4)) {
        return -1;
    }
    else if(!strncmp(cmd, "fg", 2)) {
        char *number = strtok(cmd, "fg ");
        if (number != NULL) {
            int n = atoi(number);
            job *j = getJob(n, JOBID);
            if (j == NULL) {
                return 0;
            }
            if (j->status == SUSPENDED || j->status == WAITINGINPUT) {
                setJobInBackground(j, TRUE, false);
            }
            else {
                setJobInBackground(j, FALSE, false);
            }
        }
        return 0;
    }
    else if (!strncmp(cmd, "jobs", 4)){
        listJobs();
    }
    else if (!strncmp(cmd, "kill", 4)){
        char *number = strtok(cmd, "kill ");
        if (number != NULL) {
            int n = atoi(number);
            killJob(n);

        }
        return 0;
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
        //tokens+=1;
    }
    tokens[i] = '\0';

    
    int flag = 0;
    if (tokens[0] != NULL) {
        flag = 1;
    }
    
    launchJob(tokens, (char*)"DEFAULT", flag, mode);
    return 0;
    //return launchJob(tokens, mode, (char*)"DEFAULT", flag);
    
}


#pragma mark - Environment Variables

/**
	evaluateEnvVariable
 // returns exit status, not what the env variable actually evaluates to
	@param expression expression to evaluate - without the (( and )); must be in form of i<5 or i++ or i+=1 etc
	@returns - for conditional statements < <= > >= EXIT_SUCCESS if true, EXIT_FAILURE if false
 */
int evaluateEnvVariable(char *expression)
{
    char *expPtr = expression;
    char *variable = expression;
    
    
    while (*expPtr != '\0') {
        if (*expPtr == '<' || *expPtr == '>' || (*expPtr == '=' && !strncmp(expPtr, "==", 2))) {
            //set comparison
            Comparison compare;
            switch (*expPtr) {
                case '<':{
                    if (*(expPtr+1) == '=') {
                        compare = LESS_THAN_EQUAL;
                    }else{
                        compare = LESS_THAN;
                    }
                }
                    break;
                case '>':{
                    if (*(expPtr+1) == '=') {
                        compare = GREATER_THAN_EQUAL;
                    }else{
                        compare = GREATER_THAN;
                    }
                }
                    break;
                case '=':
                    if (*(expPtr+1) == '=') {
                        compare = EQUIVALENT;
                    }
                default:
                    break;
            }
            
            //get variable
            *expPtr = '\0';
            envVar *var = find_var(variable);
            size_t leftValue = var->value;
            
            expPtr++;
            variable = expPtr;
            // get rest of comparison
            while (!isalnum(*expPtr) && *expPtr != '\0') {
                expPtr++;
            }
            if (*expPtr == '\0') {
                printf("error with loop expression");
                return EXIT_FAILURE;
            }
            
            size_t rightValue;
            if (isalpha(*expPtr)) {
                envVar *right = find_var(expPtr);
                rightValue = right->value;
            }else{
                rightValue = atoi(expPtr);
            }
            
            // Do the comparison
            if ( (compare == GREATER_THAN && leftValue > rightValue) ||
                (compare == GREATER_THAN_EQUAL && leftValue >= rightValue) ||
                (compare == LESS_THAN && leftValue < rightValue) ||
                (compare == LESS_THAN_EQUAL && leftValue <= rightValue) ||
                (compare == EQUIVALENT && leftValue == rightValue) ){
                printf("comparison is correct\n");
                return EXIT_SUCCESS;
            }else{
                printf("comparison is not true\n");
                return EXIT_FAILURE;
            }
        }else if (*expPtr == '+' || *expPtr == '-') {
            // evaluation
            Operation operate;
            
            switch (*expPtr) {
                case '+':{
                    if (*(expPtr +1) == '+') {
                        operate = PLUS_ONE;
                    }
                    if (*(expPtr+1) == '=') {

                        operate = PLUS_EQUAL;
                    }
                }
                    break;
                case '-':{
                    if (*(expPtr +1) == '-') {
                        operate = MINUS_ONE;
                    }
                    if (*(expPtr+1) == '=') {
                        operate = MINUS_EQUAL;
                    }
                }
                default:
                    break;
            }
            //get variable
            *expPtr = '\0';
            envVar *setVar = find_var(variable);
                        
            expPtr++;
            variable = expPtr;
            // get rest of comparison
            while (!isdigit(*expPtr) && *expPtr != '\0') {
                expPtr++;
            }
            
            size_t rightValue;
            if (operate != PLUS_ONE && operate != MINUS_ONE && *expPtr == '\0') {
                //error if it's not += or -= without a number
                printf("error with operation");
                return EXIT_FAILURE;
            }else{
                rightValue = atoi(expPtr);    
            }
            
            // set the variable
            if (operate == PLUS_ONE) {
                setVar->value++;
            }else if(operate == MINUS_ONE){
                setVar->value--;
            }else if(operate == MINUS_EQUAL){
                setVar->value -= rightValue;
            }else if(operate == PLUS_EQUAL){
                setVar->value += rightValue;
            }else{
                printf("error with syntax\n");
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;

        }else{
            expPtr++;
        }
    }

    return EXIT_SUCCESS;
}


int setEnvironmentVariable(char* variable, char*operand){
    if (!strncmp(variable, "PATH", 4)) {
        setExecPath(operand);
    } 
    else if (!strncmp(variable, "DATA", 4)) {
        setDataPath(operand);
    }else if (isalnum(*variable)) /*if(*variable >= 'A' && *variable <= 'Z')*/
    {
        struct envVar *v = find_var(variable);
        int val = atoi(operand);
        
        if (v != NULL) {
            v->value = val;
        }else{
            // insert into hash table
            struct envVar *s = (envVar*)malloc(sizeof(struct envVar) +1);
            s->value = atoi(operand);
            strcpy(s->name, variable);
            add_var(s);
            
            //HASH_ADD_STR(sh142Vars, name, s);
            
            /*
            envVar newVar;
            newVar.name = variable;
            newVar.value = atoi(operand);
            v = &newVar;
            add_var(v); */ 
        }
        
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
