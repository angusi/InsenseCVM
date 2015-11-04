//
// Insense Virtual Machine
// ScopeStack
//

#include "ScopeStack.h"


ScopeStack_PNTR ScopeStack_enterScope(ScopeStack_PNTR this) {
    if(this == NULL) {
        //No stack yet.
        this = IteratedList_constructList();
    }

    ScopeLevel_PNTR newLevel = IteratedList_constructList();
    IteratedList_insertElement(this, newLevel);

    return this;
}

void ScopeStack_exitScope(ScopeStack_PNTR this) {
    //Elements are added at the front of the list, so "pop" the first one.
    if(this->first != NULL) {
        IteratedList_removeElement(this, this->first);
    }
}
