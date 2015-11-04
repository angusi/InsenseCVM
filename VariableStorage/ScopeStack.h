//
// Insense Virtual Machine
// ScopeStack
//

#ifndef CVM_SCOPESTACK_H
#define CVM_SCOPESTACK_H

#include "../Collections/Stack.h"

typedef IteratedList_PNTR ScopeStack_PNTR, ScopeLevel_PNTR;

ScopeStack_PNTR ScopeStack_enterScope(ScopeStack_PNTR this);
void ScopeStack_exitScope(ScopeStack_PNTR this);
//void ScopeStack_size(ScopeStack_PNTR this);

#endif //CVM_SCOPESTACK_H
