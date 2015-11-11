/*
 * 
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

#include <stdio.h>
#include "../ScopeStack/ScopeStack.h"              // For testing
#include "../GC/GC_mem.h"                          // For memory cleanup
#include "ANSI-Colours.h"                          // For test results
#include "../Logger/Logger.h"                      // Init log for stack/GC's logging
#include "../BytecodeTable.h"

bool testConstructor();
bool testDeclareStoreLoad();

int main(int argc, char* argv[]) {

    GC_init();
    log_init();

    if (argc == 2) {
        log_setLogLevel(argv[1]);
    }

    int passed = 0;
    int failed = 0;

    if(testConstructor()) passed++;
    else failed++;

    if(testDeclareStoreLoad()) passed++;
    else failed++;


    printf("\n---\n\n"ANSI_COLOR_GREEN "%d passed" ANSI_COLOR_RESET "/" ANSI_COLOR_RED "%d failed" ANSI_COLOR_RESET "\n", passed, failed);

    return failed;
}

bool testConstructor() {
    bool result;

    ScopeStack_PNTR scopeStack = ScopeStack_enterScope(NULL);

    if(scopeStack == NULL || ScopeStack_size(scopeStack) != 1) {
        result = false;
    } else {
        result = true;
    }

    ScopeStack_PNTR newScopeStack = ScopeStack_enterScope(scopeStack);

    if(scopeStack != newScopeStack || ScopeStack_size(newScopeStack) != 2) {
        result &= false;
    } else {
        result &= true;
    }

    ScopeStack_PNTR otherStack = ScopeStack_enterScope(NULL);

    if(otherStack == scopeStack || ScopeStack_size(otherStack) != 1) {
        result &= false;
    } else {
        result &= true;
    }

    if(result) {
        printf(ANSI_COLOR_GREEN "Test passed - SCOPE STACK CONSTRUCTOR" ANSI_COLOR_RESET "\n");
    } else {
        printf(ANSI_COLOR_RED "Test failed - SCOPE STACK CONSTRUCTOR" ANSI_COLOR_RESET "\n");
    }

    ScopeStack_exitScope(otherStack);
    ScopeStack_exitScope(newScopeStack);
    ScopeStack_exitScope(scopeStack);

    return result;
}

bool testDeclareStoreLoad() {
    bool result;

    ScopeStack_PNTR scopeStack = ScopeStack_enterScope(NULL);
    ScopeStack_declare(scopeStack, "test1", BYTECODE_TYPE_INTEGER);

    int* value = GC_alloc(sizeof(int), false);
    *value = 42;
    ScopeStack_store(scopeStack, "test1", value);

    int* loadedValue = ScopeStack_load(scopeStack, "test1");

    if(*loadedValue == 42) {
        result = true;
    } else {
        result = false;
    }

    if(result) {
        printf(ANSI_COLOR_GREEN "Test passed - SCOPE STACK DECLARE STORE LOAD" ANSI_COLOR_RESET "\n");
    } else {
        printf(ANSI_COLOR_RED "Test failed - SCOPE STACK DECLARE STORE LOAD" ANSI_COLOR_RESET "\n");
    }

    ScopeStack_exitScope(scopeStack);

    return result;
}