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
#include "ListMap.h"
#include "../GC/GC_mem.h"
#include "../Logger/Logger.h"

void ListMap_decRef(ListMapEntry_PNTR pntr);

ListMap_PNTR ListMap_constructor() {
    return IteratedList_constructList();
}

void ListMap_declare(ListMap_PNTR hashList, char *key) {
    ListMapEntry_PNTR newEntry = GC_alloc(sizeof(ListMapEntry_s), true);
    newEntry->key = GC_alloc(strlen(key) + 1, false);
    strncpy(newEntry->key, key, strlen(key));

    //No need, since GC_alloc 0's memory for us:
    //newEntry->value = NULL;

    newEntry->decRef = ListMap_decRef;

    IteratedList_insertElement(hashList->first->payload, newEntry);
}

void* ListMap_get(ListMap_PNTR hashList, char *key) {
    IteratedListNode_PNTR scopeStackNode = hashList->first;
    while(scopeStackNode!=NULL){
        IteratedList_PNTR scopeLevel = scopeStackNode->payload;
        IteratedListNode_PNTR scopeLevelNode = scopeLevel->first;
        while(scopeLevelNode!=NULL) {
            ListMapEntry_PNTR thisVar = scopeLevelNode->payload;
            if(!strcmp(thisVar->key, key)) {
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
    return strcmp(((ListMapEntry_PNTR)element)->key, (char*)key);
}

void ListMap_put(ListMap_PNTR hashList, char *key, void *value) {
    ListMapEntry_PNTR varElement = NULL;
    IteratedListNode_PNTR currentStackEntry = hashList->first;

    for(int i = 0; i < IteratedList_getListLength(hashList); i++) {
        if((varElement = IteratedList_searchList(currentStackEntry->payload, HashList_isDesiredElement, key)) != NULL) {
            break;
        } else {
            currentStackEntry = currentStackEntry->tail;
        }
    }

    if(varElement == NULL) {
        log_logMessage(ERROR, "Variable Store", "Undeclared variable %s", key);
        return;
    }

    GC_assign(&(varElement->value), value);
    GC_decRef(value);
}

void ListMap_decRef(ListMapEntry_PNTR pntr) {
    GC_decRef(pntr->key);
    GC_decRef(pntr->value);
}