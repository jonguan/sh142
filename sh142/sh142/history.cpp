//
//  history.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "history.h"

void loadPreviousCommandFromHistory(char dest[], int* cmdEnd) {
    //historyViewIdx = (historyViewIdx - 1) % CMD_HISTORY_LEN;
    
    
    /*for (int i = 0; i < numEntries; i++) {
        printf("CMD%d: '%s'\n", i, cmdHistory[i]);
    }*/
    
    if (--historyViewIdx < 0) {
        historyViewIdx = 0;
        return;
    }
    loadCommandFromHistory(dest, cmdEnd, historyViewIdx);
}

void loadNextCommandFromHistory(char dest[], int* cmdEnd) {
    //historyViewIdx = (historyViewIdx + 1) % CMD_HISTORY_LEN;
    if (++historyViewIdx > numEntries - 1) {
        historyViewIdx = numEntries - 1;
        return;
    }
    loadCommandFromHistory(dest, cmdEnd, historyViewIdx);
}

void loadCommandFromHistory(char dest[], int* cmdEnd, int idx) {
    //strcpy(dest, cmdHistory[idx]);
    //printf("\n%s", dest);
    
    int i = 0;
    printf("\n");
    for (char *c = cmdHistory[idx]; *c != '\0'; c++) {
        dest[i++] = *c;
        printf("%c", *c);
    }
    *cmdEnd = i;
}

void saveCommandToHistory(char source[]) {
    strcpy(cmdHistory[historyIdx], source);
    if (++numEntries > CMD_HISTORY_LEN) {
        numEntries = CMD_HISTORY_LEN;
    }
    
    historyIdx = (historyIdx + 1) % CMD_HISTORY_LEN;
    historyViewIdx = numEntries;
}