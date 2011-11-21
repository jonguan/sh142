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

void loadPreviousCommandFromHistory(char dest[], int* cmdEnd);
void loadNextCommandFromHistory(char dest[], int* cmdEnd);
void loadCommandFromHistory(char dest[], int* cmdEnd, int idx);

void saveCommandToHistory(char source[]);


#endif
