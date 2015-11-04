//
// Insense Virtual Machine
// VariableStorage
//

#include <stddef.h>
#include "VariableStorage.h"
#include "../GC/GC_mem.h"
#include "ScopeStack.h"

void VariableStorage_declare(ScopeStack_PNTR scopeStack, char *name, int type) {
    VariableStorage_PNTR newVar = GC_alloc(sizeof(VariableStorage_s), true);
    newVar->identifier = GC_alloc(strlen(name)+1, false);
    strncpy(newVar->identifier, name, strlen(name));

    newVar->type = type;

    //No need, since GC_alloc 0's memory for us:
    //newVar->value = NULL;

    IteratedList_insertElement(scopeStack->first->payload, newVar);
}

VariableStorage_PNTR VariableStorage_load(ScopeStack_PNTR scopeStack, char *name) {
    IteratedListNode_PNTR scopeStackNode = scopeStack->first;
    while(scopeStackNode!=NULL){
        ScopeLevel_PNTR scopeLevel = scopeStackNode->payload;
        IteratedListNode_PNTR scopeLevelNode = scopeLevel->first;
        while(scopeLevelNode!=NULL) {
            VariableStorage_PNTR thisVar = scopeLevelNode->payload;
            if(!strcmp(thisVar->identifier, name)) {
                //NEGATE the strcmp, as it returns 0 on match!
                return thisVar;
            }
            scopeLevelNode = scopeLevelNode->tail;
        }
        scopeStackNode = scopeStackNode->tail;
    }

    //Exhausted entire stack. No match.
    return NULL;
}