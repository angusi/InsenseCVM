//
// Insense Virtual Machine
// VariableStorage
//

#ifndef CVM_VARIABLESTORAGE_H
#define CVM_VARIABLESTORAGE_H

#include "ScopeStack.h"

typedef struct VariableStorage {
    char* identifier;
    void* value;
    int type; //BYTECODE_TYPE_<type>
} VariableStorage_s, *VariableStorage_PNTR;

void VariableStorage_declare(ScopeStack_PNTR scopeStack, char* name, int type);
VariableStorage_PNTR VariableStorage_load(ScopeStack_PNTR scopeStac, char* name);

#endif //CVM_VARIABLESTORAGE_H
