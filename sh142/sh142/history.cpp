//
//  history.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "history.h"

void loadPreviousCommandFromHistory(char dest[]) {
    //historyViewIdx = (historyViewIdx - 1) % CMD_HISTORY_LEN;
    if (--historyIdx < 0) {
        historyIdx = 0;
        return;
    }
    loadCommandFromHistory(dest, historyViewIdx);
}

void loadNextCommandFromHistory(char dest[]) {
    //historyViewIdx = (historyViewIdx + 1) % CMD_HISTORY_LEN;
    if (++historyIdx > CMD_HISTORY_LEN - 1) {
        historyIdx = CMD_HISTORY_LEN - 1;
        return;
    }
    loadCommandFromHistory(dest, historyViewIdx);
}

void loadCommandFromHistory(char dest[], int idx) {
    strcpy(dest, cmdHistory[idx]);
    printf("%s", dest);
    
    /*int i = 0;
    for (char *c = cmdHistory[idx]; *c != '\0'; c++) {
        dest[i++] = *c;
        printf("%c", *c);
    }*/
}

void saveCommandToHistory(char source[]) {
    strcpy(cmdHistory[historyIdx], source);
    printf("Pre index: %d", historyIdx);
    if (++numEntries > CMD_HISTORY_LEN) {
        numEntries = CMD_HISTORY_LEN;
    }
    
    /*int i = 0;
    for (char *c = source; *c != '\0'; c++) {
        cmdHistory[historyIdx][i++] = *c;
    }*/
    historyIdx = (historyIdx + 1) % numEntries;
    printf("Post index: %d", historyIdx);
}