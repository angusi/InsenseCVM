#include <stdio.h>
#include "../Collections/Stack.h"    // For testing
#include "../GC/GC_mem.h"            // For memory cleanup
#include "ANSI-Colours.h"            // For test results
#include "../Logger/Logger.h"        // Init log for stack/GC's logging

bool testConstructor();
bool testPushItemOntoStack();
bool testPopItemFromStack();
bool testPushItemsOntoStack();
bool testPopItemsFromStack();
bool testPeekAtStack();

int main(int argc, char* argv[]) {

    GC_init();
    log_init();

    if(argc==2) {
        log_setLogLevel(argv[1]);
    }

    int passed = 0;
    int failed = 0;

    if(testConstructor()) passed++;
    else failed++;

    if(testPushItemOntoStack()) passed++;
    else failed++;

    if(testPushItemsOntoStack()) passed++;
    else failed++;

    if(testPopItemFromStack()) passed++;
    else failed++;

    if(testPopItemsFromStack()) passed++;
    else failed++;

    if(testPeekAtStack()) passed++;
    else failed++;

    printf("\n---\n\n"ANSI_COLOR_GREEN "%d passed" ANSI_COLOR_RESET "/" ANSI_COLOR_RED "%d failed" ANSI_COLOR_RESET "\n", passed, failed);

    return failed;
}

bool testConstructor() {
    bool result;

    Stack_PNTR stack = Stack_constructor();

    if(stack == NULL) {
        printf(ANSI_COLOR_RED "Test failed - STACK CONSTRUCTOR" ANSI_COLOR_RESET "\n" );
        result = false;
    } else {
        printf(ANSI_COLOR_GREEN "Test passed - STACK CONSTRUCTOR (%p)" ANSI_COLOR_RESET "\n" , (void*)stack);
        result = true;

        GC_decRef(stack);
    }
    return result;
}

bool testPushItemOntoStack() {
    bool result;

    Stack_PNTR stack = Stack_constructor();

    int* someInteger = GC_alloc(sizeof(int), false);
    *someInteger = 42;

    Stack_push(stack, someInteger);

    if(stack->stackTop != 1) {
        printf(ANSI_COLOR_RED "Test failed - STACK PUSH" ANSI_COLOR_RESET "\n");
        result = false;
    } else {
        printf(ANSI_COLOR_GREEN "Test passed - STACK PUSH (%p)" ANSI_COLOR_RESET "\n", (void*)someInteger);
        result = true;
    }

    GC_decRef(stack);

    return result;

}

bool testPushItemsOntoStack() {
    bool result;

    Stack_PNTR stack = Stack_constructor();

    int* someInteger = GC_alloc(sizeof(int), false);
    *someInteger = 42;

    int* someOtherInteger = GC_alloc(sizeof(int), false);
    *someOtherInteger = 43;

    Stack_push(stack, someInteger);

    if(stack->stackTop != 1) {
        result = false;
    } else {
        result = true;
    }

    Stack_push(stack, someOtherInteger);

    if(stack->stackTop != 2) {

        result &= false;
    } else {

        result &= true;
    }

    GC_decRef(stack);

    if(result) {
        printf(ANSI_COLOR_GREEN "Test passed - STACK PUSH MULTIPLE (%p, %p)" ANSI_COLOR_RESET "\n", (void*)someInteger, (void*)someOtherInteger);
    } else {
        printf(ANSI_COLOR_RED "Test failed - STACK PUSH MULTIPLE" ANSI_COLOR_RESET "\n");
    }
    return result;
}

bool testPopItemFromStack() {
    bool result;

    Stack_PNTR stack = Stack_constructor();

    int* someInteger = GC_alloc(sizeof(int), false);
    *someInteger = 42;

    Stack_push(stack, someInteger);
    int* retrievedInteger = Stack_pop(stack);

    if(stack->stackTop != 0 || *retrievedInteger != 42) {
        printf(ANSI_COLOR_RED "Test failed - STACK POP" ANSI_COLOR_RESET "\n");
        result = false;
    } else {
        printf(ANSI_COLOR_GREEN "Test passed - STACK POP (%p,%p)" ANSI_COLOR_RESET "\n", (void*)someInteger, (void*)retrievedInteger);
        result = true;
    }

    GC_decRef(retrievedInteger);

    GC_decRef(stack);

    return result;
}

bool testPopItemsFromStack() {
    bool result;

    Stack_PNTR stack = Stack_constructor();

    int* someInteger = GC_alloc(sizeof(int), false);
    *someInteger = 42;

    int* someOtherInteger = GC_alloc(sizeof(int), false);
    *someOtherInteger = 43;

    Stack_push(stack, someInteger);
    Stack_push(stack, someOtherInteger);

    int* retrievedOtherInteger = Stack_pop(stack);

    if(stack->stackTop != 1 || *retrievedOtherInteger != 43) {
        result = false;
    } else {
        result = true;
    }

    GC_decRef(retrievedOtherInteger);

    int* retrievedInteger = Stack_pop(stack);

    if(stack->stackTop != 0 || *retrievedInteger != 42) {
        result &= false;
    } else {
        result &= true;
    }

    if(result) {
        printf(ANSI_COLOR_GREEN "Test passed - STACK POP MULTIPLE ((%p,%p), (%p,%p))" ANSI_COLOR_RESET "\n",
               (void*)someInteger, (void*)retrievedInteger, (void*)someOtherInteger, (void*)retrievedOtherInteger);
    } else {
        printf(ANSI_COLOR_RED "Test failed - STACK POP MULTIPLE" ANSI_COLOR_RESET "\n");
    }



    GC_decRef(stack);

    return result;
}

bool testPeekAtStack() {
    bool result;

    Stack_PNTR stack = Stack_constructor();

    int* someInteger = GC_alloc(sizeof(int), false);
    *someInteger = 42;

    Stack_push(stack, someInteger);
    int* retrievedInteger = Stack_peek(stack);

    if(stack->stackTop != 1 || *retrievedInteger != 42) {
        printf(ANSI_COLOR_RED "Test failed - STACK PEEK %d %d" ANSI_COLOR_RESET "\n", stack->stackTop, *retrievedInteger);
        result = false;
    } else {
        printf(ANSI_COLOR_GREEN "Test passed - STACK PEEK (%p,%p)" ANSI_COLOR_RESET "\n", (void*)someInteger, (void*)retrievedInteger);
        result = true;
    }

    GC_decRef(retrievedInteger);

    GC_decRef(stack);

    return result;
}