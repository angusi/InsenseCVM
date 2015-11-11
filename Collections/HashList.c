/*
 * Code for declaring/storing and loading variables from a scope stack.
 *
 * Copyright (c) 2015, Angus Ireland
 * School of Computer Science, St. Andrews University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <stddef.h>
#include "HashList.h"
#include "../GC/GC_mem.h"
#include "../Logger/Logger.h"

void HashList_declare(HashList_PNTR hashList, char *name, int type) {
    HashListEntry_PNTR newVar = GC_alloc(sizeof(HashListEntry_s), true);
    newVar->identifier = GC_alloc(strlen(name)+1, false);
    strncpy(newVar->identifier, name, strlen(name));

    newVar->type = type;

    //No need, since GC_alloc 0's memory for us:
    //newVar->value = NULL;

    IteratedList_insertElement(hashList->first->payload, newVar);
}

void* HashList_get(HashList_PNTR hashList, char *name) {
    IteratedListNode_PNTR scopeStackNode = hashList->first;
    while(scopeStackNode!=NULL){
        IteratedList_PNTR scopeLevel = scopeStackNode->payload;
        IteratedListNode_PNTR scopeLevelNode = scopeLevel->first;
        while(scopeLevelNode!=NULL) {
            HashListEntry_PNTR thisVar = scopeLevelNode->payload;
            if(!strcmp(thisVar->identifier, name)) {
                //NEGATE the strcmp, as it returns 0 on match!
                return thisVar->value;
            }
            scopeLevelNode = scopeLevelNode->tail;
        }
        scopeStackNode = scopeStackNode->tail;
    }

    //Exhausted entire stack. No match.
    return NULL;
}

int HashList_isDesiredElement(void *element, void *key) {
    return strcmp(((HashListEntry_PNTR)element)->identifier, (char*)key);
}

void HashList_put(HashList_PNTR hashList, char *name, void *value) {
    HashListEntry_PNTR varElement = NULL;
    IteratedListNode_PNTR currentStackEntry = hashList->first;

    for(int i = 0; i < IteratedList_getListLength(hashList); i++) {
        if((varElement = IteratedList_searchList(currentStackEntry->payload, HashList_isDesiredElement, name)) != NULL) {
            break;
        } else {
            currentStackEntry = currentStackEntry->tail;
        }
    }

    if(varElement == NULL) {
        log_logMessage(ERROR, "Variable Store", "Undeclared variable %s", name);
        return;
    }

    GC_assign(&(varElement->value), value);
}
