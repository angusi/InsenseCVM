/*
 * Variable Scope stack definitions
 *
 * Functions here manage scoping within various components
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

#include <stdlib.h>
#include <string.h>
#include "ScopeStack.h"
#include "GC/GC_mem.h"
#include "Logger.h"

ScopeLevel_PNTR scopeStack_enterScope(ScopeLevel_PNTR parentScope) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Scope Stack", "Entering Scope");
#endif
    ScopeLevel_PNTR newLevel = malloc(sizeof(ScopeLevel_s));
    newLevel->firstEntry = NULL;
    newLevel->parentScope = parentScope;

    return newLevel;
}

ScopeLevel_PNTR scopeStack_exitScope(ScopeLevel_PNTR currentScope) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Scope Stack", "Exiting Scope");
#endif
    if(currentScope->firstEntry != NULL) {
        ScopeEntry_PNTR* currentItem = malloc(sizeof(__intptr_t));
        ScopeEntry_PNTR* nextItem = malloc(sizeof(__intptr_t));
        memcpy(nextItem, &(currentScope->firstEntry), sizeof(__intptr_t));
        while((*currentItem)->nextEntry != NULL) {
            currentItem = memcpy(currentItem, nextItem, sizeof(__intptr_t));
            memcpy(nextItem, &((*currentItem)->nextEntry), sizeof(__intptr_t));
            if((*currentItem)->GC_item) {
                GC_decRef((*currentItem)->value);
            }
            free(*currentItem);
        }
        free(currentItem);
        free(nextItem);
    }

    ScopeLevel_PNTR parentScope = currentScope->parentScope;
    free(currentScope);
    return parentScope;
}