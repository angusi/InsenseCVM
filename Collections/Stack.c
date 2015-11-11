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
#include "Stack.h"
#include "../GC/GC_mem.h"
#include "../Logger/Logger.h"

static void Stack_decRef(Stack_PNTR pntr);
static void StackEntry_decRef(StackEntry_PNTR pntr);

Stack_PNTR Stack_constructor(){
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Stack", "Constructing Stack");
#endif
    Stack_PNTR this = (Stack_PNTR) GC_alloc(sizeof(Stack_s), true);
    if(this == 0){
        log_logMessage(ERROR, "Stack", "Failed to construct Stack");
        return 0;
    }
    this->storage = IteratedList_constructList();
    this->decRef = Stack_decRef;
    this->stackTop = 0;
    return(this);
}

void Stack_push(Stack_PNTR this, void* item) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Stack", "Pushing %p onto stack at %p", item, this);
#endif

    StackEntry_PNTR newEntry = GC_alloc(sizeof(StackEntry_s), true);
    newEntry->decRef = StackEntry_decRef;
    newEntry->object = item;

    IteratedList_insertElement(this->storage, newEntry);
    this->stackTop++;
}

void* Stack_pop(Stack_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Stack", "Popping from stack at %p", this);
#endif
    if(this->stackTop <= 0) {
        log_logMessage(ERROR, "Stack", "Underflow: Cannot pop from empty stack");
        return NULL;
    }

    StackEntry_PNTR element = IteratedList_getElementN(this->storage, 0);
    GC_incRef(element->object);
    IteratedList_removeElement(this->storage, element);
    this->stackTop--;
    return element->object;

}

void* Stack_peek(Stack_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Stack", "Peeking at stack at %p", this);
#endif
    if(this->stackTop <= 0) {
        log_logMessage(ERROR, "Stack", "Underflow: Cannot peek at empty stack");
        return NULL;
    }

    StackEntry_PNTR element = IteratedList_getElementN(this->storage, 0);
    GC_incRef(element->object);

    return element->object;
}

void Stack_clear(Stack_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Stack", "Clearing stack at %p", this);
#endif
    while(this->stackTop > 0) {
        Stack_pop(this);
    }
}

// decRef function is called when ref count to a Stack object is zero
// before freeing memory for Stack object
static void Stack_decRef(Stack_PNTR this){
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Stack", "Decrementing reference to stack");
#endif
    if(!this->stackTop == 0) {
        Stack_clear(this); // forces decRef on IteratedList content (node payload)
    }
}

static void StackEntry_decRef(StackEntry_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Stack", "Decrementing reference to stack entry");
#endif
    GC_decRef(this->object);
}