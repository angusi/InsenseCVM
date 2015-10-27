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

#ifndef CVM_SCOPESTACK_H
#define CVM_SCOPESTACK_H

#include <stdbool.h>

typedef struct ScopeEntry {
    struct ScopeEntry* nextEntry;
    const char* name;
    void* value;
    bool GC_item;
} ScopeEntry_s, *ScopeEntry_PNTR;

typedef struct ScopeLevel {
    struct ScopeLevel* parentScope;
    ScopeEntry_PNTR firstEntry;
} ScopeLevel_s, *ScopeLevel_PNTR;

ScopeLevel_PNTR scopeStack_enterScope(ScopeLevel_PNTR parentScope);
ScopeLevel_PNTR scopeStack_exitScope(ScopeLevel_PNTR currentScope);

#endif //CVM_SCOPESTACK_H
