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

#ifndef CVM_SCOPESTACK_H
#define CVM_SCOPESTACK_H

#include "../Collections/Stack.h"
#include "../Collections/HashList.h"

typedef IteratedList_PNTR ScopeStack_PNTR, ScopeLevel_PNTR;

ScopeStack_PNTR ScopeStack_enterScope(ScopeStack_PNTR this);
void ScopeStack_exitScope(ScopeStack_PNTR this);
int ScopeStack_size(ScopeStack_PNTR this);

void ScopeStack_declare(ScopeStack_PNTR this, char *name, int type);
void* ScopeStack_load(ScopeStack_PNTR this, char *name);
void ScopeStack_store(ScopeStack_PNTR this, char *name, void *value);

#endif //CVM_SCOPESTACK_H
