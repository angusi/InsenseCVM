/*
 * Component operation declarations.
 *
 * Components may be instantiated and run from this code.
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

#ifndef CVM_COMPONENT_H
#define CVM_COMPONENT_H

#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include "GC/GC_mem.h"
#include "Logger/Logger.h"
#include "Collections/Stack.h"
#include "ScopeStack/ScopeStack.h"
#include "Channels/channel.h"
#include "TypedObject.h"

typedef struct Component Component_s, *Component_PNTR;
struct Component {
    void (*decRef)(Component_PNTR component);
    char* name;
    FILE* sourceFile;
    pthread_t threadId;
    IteratedList_PNTR parameters;
    ScopeStack_PNTR scopeStack;
    Stack_PNTR dataStack;
    Stack_PNTR waitComponents;
    ListMap_PNTR channels;
    bool stop;
};

Component_PNTR component_newComponent(char *sourceFile, IteratedList_PNTR params);
void* component_run(void* this);
char* component_getName(Component_PNTR this);

//TODO: Should these be declared at head of C file instead of in H file?
void component_cleanUpAndStop(Component_PNTR this, void* __retval);
void component_enterScope(Component_PNTR this);
void component_exitScope(Component_PNTR this);
TypedObject_PNTR component_readData(Component_PNTR this);
char* component_readString(Component_PNTR this);
void* component_readNBytes(Component_PNTR this, size_t nBytes);
Component_PNTR component_call(Component_PNTR this);
void component_constructor(Component_PNTR this);
void component_declare(Component_PNTR this);
void component_store(Component_PNTR this);
void component_load(Component_PNTR this);
void component_component(Component_PNTR this);
void component_push(Component_PNTR this);
int component_skipToNext(Component_PNTR this, int bytecode);
void component_jump(Component_PNTR this);
void component_behaviourJump(Component_PNTR this);
void component_expression(Component_PNTR this, int bytecode_op);
void component_not(Component_PNTR this);
void component_stop(Component_PNTR this);
void component_ifClause(Component_PNTR this);
void component_elseClause(Component_PNTR this);
void component_connect(Component_PNTR this);
void component_disconnect(Component_PNTR this);
void component_send(Component_PNTR this);
void component_receive(Component_PNTR this);

#endif //CVM_COMPONENT_H
