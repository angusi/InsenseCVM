/*
 * Scope Stacks manage variable scoping.
 * Scope levels are created to store variables, and when they are destroyed those variables are also destroyed.
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

#include "ScopeStack.h"
#include "../Logger/Logger.h"
#include "../GC/GC_mem.h"


ScopeStack_PNTR ScopeStack_enterScope(ScopeStack_PNTR this) {
    if(this == NULL) {
        //No stack yet.
        this = IteratedList_constructList();
    }

    ListMap_PNTR newLevel = ListMap_constructor();
    IteratedList_insertElement(this, newLevel);

    GC_decRef(newLevel);

    return this;
}

void ScopeStack_exitScope(ScopeStack_PNTR this) {
    //Elements are added at the front of the list, so "pop" the first one.
    if(this->first != NULL) {
        IteratedList_removeElement(this, this->first->payload);
    }
}

int ScopeStack_size(ScopeStack_PNTR this) {
    return IteratedList_getListLength(this);
}


void ScopeStack_declare(ScopeStack_PNTR this, char *name) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "ScopeStack", "Declaring %s in Scope Stack %p", name, this);
#endif
    return ListMap_declare(this->first->payload, name);
}

void* ScopeStack_load(ScopeStack_PNTR this, char *name) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "ScopeStack", "Loading %s from Scope Stack %p", name, this);
#endif

    IteratedListNode_PNTR first = this->first;
    IteratedListNode_PNTR thisLevel = first;

    void* element = ListMap_get(thisLevel->payload, name);
    if(element != NULL) {
        return element;
    } else {
        thisLevel = thisLevel->tail;
        while(thisLevel != first) {
            element = ListMap_get(thisLevel->payload, name);
            if(element != NULL) {
                return element;
            } else {
                thisLevel = thisLevel->tail;
            }
        }
    }

    return NULL;
}

void ScopeStack_store(ScopeStack_PNTR this, char *name, void *value) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "ScopeStack", "Store %p as %s in Scope Stack %p", value, name, this);
#endif

    IteratedListNode_PNTR first = this->first;
    IteratedListNode_PNTR thisLevel = first;

    if(ListMap_put(thisLevel->payload, name, value)) {
        return;
    } else {
        thisLevel = thisLevel->tail;
        while(thisLevel != first) {
            if(ListMap_put(thisLevel->payload, name, value)) {
                return;
            } else {
                thisLevel = thisLevel->tail;
            }
        }
    }

    log_logMessage(ERROR, "Variable Store", "Undeclared variable %s", name);
    return;
}
