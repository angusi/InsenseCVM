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
#include "InsenseRuntimeCVM/StandardFunctions.h"
#include "Logger/Logger.h"
#include "Collections/Stack.h"
#include "ScopeStack/ScopeStack.h"
#include "Channels/channel.h"
#include "TypedObject.h"
#include "Channels/my_mutex.h"

/**
 * The main "Component" representation.
 *
 * This struct holds pointers to all the data an Insense Component needs to operate.
 * All component instance methods will require a pointer to an instance of this struct.
 */
typedef struct Component Component_s, *Component_PNTR;
struct Component {
    void (*decRef)(Component_PNTR component); //!< A pointer to the garbage collection function. Automatically set by constructor.
    char* name;                               //!< Pointer to a char* with the Component's friendly name.
    FILE* sourceFile;                         //!< Pointer to a file object with this component's bytecode source.
    pthread_t threadId;                       //!< On Unix, the thread ID that this component is running in.
    IteratedList_PNTR parameters;             //!< A list of parameters passed into this component.
    ScopeStack_PNTR scopeStack;               //!< The scope stack, where local variables are stored.
    Stack_PNTR dataStack;                     //!< The data stack, where date being operated on is stored.
    Stack_PNTR waitComponents;                //!< Identifiers/Pointers to components started by this component, that must be waited on before this Component may terminate.
    ListMap_PNTR channels;                    //!< List of channels used for inter-component communication.
    ListMap_PNTR procs;                       //!< List of procedures and their byte positions in this component.
    bool stop;                                //!< If true, Component will terminate on next instruction.
    bool running;                             //!< Certain operations require the component to be fully initialised. True on this flag indicates this status.
    bool inProject;                           //!< Project blocks need skipping out of at the end, so this marks if a project block is being executed.
};

/**
 * Instantiate a new Component instance.
 *
 * @param[in] sourceFile The path of the file containing this Component's bytecode source.
 * @param[in] params     A list of parameters, in the order expected by the component.
 * 
 * @return A newly created Component_PNTR. This object will require Garbage Collection.
 */
Component_PNTR component_newComponent(char *sourceFile, IteratedList_PNTR params);
void* component_run(void* this);

//Implementation-specific methods:
void Component_waitForExit(Component_PNTR waitComponent);
void Component_exit(void* __retval);
void Component_create(Component_PNTR newComponent);

#endif //CVM_COMPONENT_H
