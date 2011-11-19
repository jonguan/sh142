//
//  history.cpp
//  sh142
//
//  Created by Torkil Aamodt on 11/18/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "history.h"

int loadPreviousCommandFromHistory() {
    historyViewIdx = (historyViewIdx - 1) % CMD_HISTORY_LEN;
    return loadCommandFromHistory(historyViewIdx);
}

int loadNextCommandFromHistory() {
    historyViewIdx = (historyViewIdx + 1) % CMD_HISTORY_LEN;
    return loadCommandFromHistory(historyViewIdx);
}

int loadCommandFromHistory(int idx) {
    return 1;
}
