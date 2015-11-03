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
    IteratedList_insertElement(this->storage, item);
    this->stackTop++;
}

void* Stack_pop(Stack_PNTR this) {
    if(this->stackTop <= 0) {
        log_logMessage(ERROR, "Stack", "Underflow: Cannot pop from empty stack");
        return NULL;
    }

    void* element = IteratedList_getElementN(this->storage, 0);
    void* item = GC_alloc(sizeof(void*), GC_mem_contains_pointers(element));
    GC_assign(&item, element);
    IteratedList_removeElement(this->storage, element);
    this->stackTop--;
    return item;

}

void* Stack_peek(Stack_PNTR this) {
    if(this->stackTop <= 0) {
        log_logMessage(ERROR, "Stack", "Underflow: Cannot pop from empty stack");
        return NULL;
    }

    void* element = IteratedList_getElementN(this->storage, 0);
    void* item = GC_alloc(sizeof(void*), GC_mem_contains_pointers(element));
    GC_assign(&item, element);

    return item;
}

void Stack_clear(Stack_PNTR this) {
    while(this->stackTop > 0) {
        Stack_pop(this);
    }
}

// decRef function is called when ref count to a Stack object is zero
// before freeing memory for Stack object
static void Stack_decRef(Stack_PNTR this){
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, "Stack", "Decrementing reference to list");
#endif
    if(!this->stackTop == 0) {
        Stack_clear(this); // forces decRef on IteratedList content (node payload)
    }
}