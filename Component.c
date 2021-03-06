/*
 * @file Component.c
 * Component operations.
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

#include <unistd.h>
#include <stdio.h>
#include "Component.h"
#include "BytecodeTable.h"
#include "Main.h"
#include "ChannelWrapper.h"
#include "Procedure.h"

static void Component_decRef(Component_PNTR pntr);

void component_cleanUpAndStop(Component_PNTR this, void* __retval);
void component_enterScope(Component_PNTR this);
void component_exitScope(Component_PNTR this);
TypedObject_PNTR component_readData(Component_PNTR this);
char* component_readString(Component_PNTR this);
void* component_readNBytes(Component_PNTR this, size_t nBytes);
char* Component_getSourceFile(char* name);
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
void component_proc(Component_PNTR this);
void component_procCall(Component_PNTR this);
void component_procReturn(Component_PNTR this);
void component_struct(Component_PNTR this);
void component_struct_constructor(Component_PNTR this);
void component_struct_load(Component_PNTR this);
void component_blockEnd(Component_PNTR this);
void component_any(Component_PNTR this);
void component_projectEntry(Component_PNTR this);
void component_projectExit(Component_PNTR this);

/**
 * Construct a new component object
 * @param[in] sourceFile String containing path to bytecode source file
 * @param[in] params Iterated List of parameters
 * @return Pointer to new component object
 */
Component_PNTR component_newComponent(char* name, char *sourceFile, IteratedList_PNTR params) {
    Component_PNTR this = GC_alloc(sizeof(Component_s), true);
    this->decRef = Component_decRef;

    size_t componentNameSize = strlen(name);
    this->name = GC_alloc(componentNameSize+1, false);
    strcat(this->name, name);

    this->sourceFile = fopen(sourceFile, "rb");
    if(this->sourceFile == NULL) {
        log_logMessage(FATAL, this->name, "Component Source file does not exist!");
        component_cleanUpAndStop(this, NULL);
    }

    this->parameters = params;

    this->dataStack = Stack_constructor();

    this->channels = ListMap_constructor();

    this->stop = false;

    this->running = false;

    log_logMessage(INFO, this->name, "Component Created");

    return this;
}

/**
 * Start a component running
 * @param[in,out] component Component instance to run.
 */
void* component_run(void* component) {
    Component_PNTR this = (Component_PNTR)component;
    log_logMessage(INFO, this->name, "Start");

    int nextByte;
    while(!this->stop && (nextByte = fgetc(this->sourceFile)) != EOF) {
        switch(nextByte) {
            case BYTECODE_ENTERSCOPE:
                component_enterScope(this);
                break;
            case BYTECODE_EXITSCOPE:
                component_exitScope(this);
                break;
            case BYTECODE_COMPONENT:
                component_component(this);
                break;
            case BYTECODE_CALL:
                component_call(this);
                break;
            case BYTECODE_DECLARE:
                component_declare(this);
                break;
            case BYTECODE_STORE:
                component_store(this);
                break;
            case BYTECODE_PUSH:
                component_push(this);
                break;
            case BYTECODE_LOAD:
                component_load(this);
                break;
            case BYTECODE_CONSTRUCTOR:
                component_constructor(this);
                break;
            case BYTECODE_ADD:
            case BYTECODE_SUB:
            case BYTECODE_MUL:
            case BYTECODE_DIV:
            case BYTECODE_MOD:
            case BYTECODE_LESS:
            case BYTECODE_LESSEQUAL:
            case BYTECODE_EQUAL:
            case BYTECODE_MOREEQUAL:
            case BYTECODE_MORE:
            case BYTECODE_UNEQUAL:
            case BYTECODE_AND:
            case BYTECODE_OR:
                component_expression(this, nextByte);
                break;
            case BYTECODE_NOT:
                component_not(this);
                break;
            case BYTECODE_STOP:
                component_stop(this);
                break;
            case BYTECODE_BEHAVIOUR_JUMP:
                component_behaviourJump(this);
                break;
            case BYTECODE_JUMP:
                component_jump(this);
                break;
            case BYTECODE_IF:
                component_ifClause(this);
                break;
            case BYTECODE_ELSE:
                component_elseClause(this);
                break;
            case BYTECODE_CONNECT:
                component_connect(this);
                break;
            case BYTECODE_DISCONNECT:
                component_disconnect(this);
                break;
            case BYTECODE_SEND:
                component_send(this);
                break;
            case BYTECODE_RECEIVE:
                component_receive(this);
                break;
            case BYTECODE_PROC:
                component_proc(this);
                break;
            case BYTECODE_BLOCKEND:
                component_blockEnd(this);
                break;
            case BYTECODE_PROCCALL:
                component_procCall(this);
                break;
            case BYTECODE_RETURN:
                component_procReturn(this);
                break;
            case BYTECODE_STRUCT:
                component_struct(this);
                break;
            case BYTECODE_ANY:
                component_any(this);
                break;
            case BYTECODE_PROJECT_ENTRY:
                component_projectEntry(this);
                break;
            case BYTECODE_PROJECT_EXIT:
                component_projectExit(this);
            default:
                log_logMessage(ERROR, this->name, "Unknown Byte Read - %u", nextByte);
                break;
        }
    }

    log_logMessage(INFO, this->name, "End");
    component_cleanUpAndStop(this, NULL);

    return NULL;
}

void component_cleanUpAndStop(Component_PNTR this, void* __retval) {
    log_logMessage(INFO, this->name, "Cleaning up Component and returning to caller.");

    if(this->waitComponents != NULL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "Waiting on started components.");
#endif
        while(Stack_size(this->waitComponents) != 0) {
            Component_PNTR waitOn = TypedObject_getObject((TypedObject_PNTR)Stack_pop(this->waitComponents));
#ifdef DEBUGGINGENABLED
            log_logMessage(DEBUG, this->name, "  Waiting on %s (%lu)", waitOn->name, waitOn->threadId);
#endif
            Component_waitForExit(waitOn);
            GC_decRef(waitOn);
        }
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "All started components stopped.");
#endif
    }

    //Copy name so we can use it in the done message, after Component has been trashed.
    char* name = malloc(strlen(this->name)+1);
    strcpy(name, this->name);
    name[strlen(this->name)] = '\0';

    GC_decRef(this);
    log_logMessage(INFO, name, "DONE. Component cleaned up.");
    free(name);
    Component_exit(__retval);
}

void component_enterScope(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "ENTERSCOPE");
#endif
    this->scopeStack = ScopeStack_enterScope(this->scopeStack);
}

void component_exitScope(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "EXITSCOPE");
#endif
    ScopeStack_exitScope(this->scopeStack);
}

void component_component(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "COMPONENT");
#endif

    char* name = component_readString(this);
    if(strcmp(name, this->name) != 0) { //Negated strcmp because 0 is match
        log_logMessage(FATAL, this->name, "Syntax error in COMPONENT - name %s does not match expected %s", name, this->name);
        component_cleanUpAndStop(this, NULL);
    }

    int number_of_interfaces = fgetc(this->sourceFile);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    %d interfaces", number_of_interfaces);
#endif
    for(int i = 0; i < number_of_interfaces; i++) {
        int number_of_channels = fgetc(this->sourceFile);
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    %d channels", number_of_channels);
#endif

        for(int j = 0; j < number_of_channels; j++) {
            int channel_direction = fgetc(this->sourceFile) ;
            if(channel_direction == BYTECODE_TYPE_IN) {
#ifdef DEBUGGINGENABLED
                log_logMessage(DEBUG, this->name, "     Channel %d: IN", j);
#endif
                channel_direction = CHAN_IN;
            } else if(channel_direction == BYTECODE_TYPE_OUT) {
#ifdef DEBUGGINGENABLED
                log_logMessage(DEBUG, this->name, "     Channel %d: OUT", j);
#endif
                channel_direction = CHAN_OUT;
            } else {
                log_logMessage(FATAL, this->name, "Syntax error in COMPONENT - channel direction unknown");
                component_cleanUpAndStop(this, NULL);
            }
            int channel_type = fgetc(this->sourceFile);
#ifdef DEBUGGINGENABLED
            log_logMessage(DEBUG, this->name, "     Channel %d: %d", j, channel_type);
#endif
            char* channel_name = component_readString(this);
#ifdef DEBUGGINGENABLED
            log_logMessage(DEBUG, this->name, "     Channel %d: %s", j, channel_name);
#endif
            Channel_PNTR new_channel = channel_create(channel_direction, TypedObject_getSize(channel_type));
            ChannelWrapper_PNTR channelWrapper = GC_alloc(sizeof(ChannelWrapper_s), false);
            channelWrapper->channel = new_channel;
            channelWrapper->type = channel_type;
            ListMap_declare(this->channels, channel_name);
            if(!ListMap_put(this->channels, channel_name, channelWrapper)) {
                log_logMessage(FATAL, this->name, "Channel List refused to store the channel for an unknown reason.");
                component_cleanUpAndStop(this, NULL);
            };
            GC_decRef(channel_name);
        }
    }
    GC_decRef(name);
}

Component_PNTR component_call(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "CALL");
#endif
    char* name = component_readString(this);
    int number_of_parameters = fgetc(this->sourceFile);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "   Calling %s", name);
#endif

    IteratedList_PNTR paramsList = NULL;
    if(number_of_parameters > 0) {
        paramsList = IteratedList_constructList();
        for (int i = 0; i < number_of_parameters; i++) {
            TypedObject_PNTR param = Stack_pop(this->dataStack);
            IteratedList_insertElement(paramsList, param);
        }
    }

    char* sourceFile = Component_getSourceFile(name);
    char* filePath = getFilePath(sourceFile);
    Component_PNTR newComponent = component_newComponent(name, filePath, paramsList);
    Component_create(newComponent);

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Component %s is at address %p", name, newComponent);
#endif

    TypedObject_PNTR newObject = TypedObject_construct(BYTECODE_TYPE_COMPONENT, newComponent);
    Stack_push(this->dataStack, newObject);

    if(this->waitComponents == NULL) {
        this->waitComponents = Stack_constructor();
    }
    Stack_push(this->waitComponents, newObject);

    GC_decRef(sourceFile);
    GC_decRef(filePath);
    GC_decRef(name);

    return newComponent;
}

void component_constructor(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "CONSTRUCTOR");
#endif

    if(this->running) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "   Skipping since already constructed");
#endif
        //Component already constructed, skip over constructor.
        component_skipToNext(this, BYTECODE_BLOCKEND);
        fgetc(this->sourceFile); //Consume the BLOCKEND
    }

    bool thisConstructor = true;
    IteratedList_PNTR readParams = NULL;

    int givenParameters = 0;
    if(this->parameters != NULL) {
        givenParameters = IteratedList_getListLength(this->parameters);
    }

    int paramsToRead = fgetc(this->sourceFile);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "  %d params", paramsToRead);
#endif
    if(paramsToRead == givenParameters) {
        readParams = IteratedList_constructList();
        for(int i = 0; i < givenParameters; i++) {
            int nextParamType = fgetc(this->sourceFile);
            TypedObject_PNTR nextParam = IteratedList_getNextElement(this->parameters);
            if(nextParamType == TypedObject_getTypeByteCode(nextParam)) {
                char *name = component_readString(this);
                IteratedList_insertElementAtTail(readParams, name);
                GC_decRef(name);
            } else {
                thisConstructor = false;
                GC_decRef(component_readString(this)); //Immediately discard data
            }
        }
    } else {
        thisConstructor = false;
        for(int i = 0; i < paramsToRead; i++) {
            fgetc(this->sourceFile);               //Skip type
            GC_decRef(component_readString(this)); //Skip name
        }
    }

    if(thisConstructor) {
        log_logMessage(INFO, this->name, "  Constructor match");
        IteratedList_rewind(readParams);
        if(this->parameters != NULL) {
            IteratedList_rewind(this->parameters);
        }
        for(int i = 0; i < paramsToRead; i++) {
            char* name = IteratedList_getNextElement(readParams);
            ScopeStack_declare(this->scopeStack, name);
            ScopeStack_store(this->scopeStack, name, IteratedList_getNextElement(this->parameters));
        }

        //Finished with these lists now, can free them up.
        if(this->parameters != NULL) {
            GC_decRef(this->parameters);
            this->parameters = NULL;
        }
        GC_decRef(readParams);

        //Component is now fully executable
        this->running = true;
    } else {
        log_logMessage(INFO, this->name, " Constructor mismatch, fastforwarding");

        if(readParams != NULL) {
            GC_decRef(readParams);
        }
        IteratedList_rewind(this->parameters);

        int nextByte = component_skipToNext(this, BYTECODE_CONSTRUCTOR);
        if( nextByte == BYTECODE_CONSTRUCTOR ) {
            component_constructor(this);
        } else {
            log_logMessage(FATAL, this->name, "Constructor not found!");
            component_cleanUpAndStop(this, NULL);
        }
    }
}

void component_declare(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "DECLARE");
#endif
    char* name = component_readString(this);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "   Declaring %s", name);
#endif

    fgetc(this->sourceFile); //We don't care about the type when declaring.
                             // (Also, type-checking has already been done by the compiler anyway.)
    ScopeStack_declare(this->scopeStack, name);
    GC_decRef(name);
}

void component_store(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "STORE");
#endif

    char *name = component_readString(this);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "   Storing %s", name);
#endif

    if(ScopeStack_store(this->scopeStack, name, Stack_pop(this->dataStack)) != 0) {
        log_logMessage(FATAL, this->name, "  Unable to store data.");
        component_cleanUpAndStop(this, NULL);
    }
    GC_decRef(name);
}

void component_push(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "PUSH");
#endif

    void* data = component_readData(this);
    if(data == NULL) {
        log_logMessage(FATAL, this->name, "Tried to push data onto stack, but no data could be read.");
        component_cleanUpAndStop(this, NULL);
    }
    Stack_push(this->dataStack, data);
}

void component_load(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "LOAD");
#endif

    char *name = component_readString(this);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "   Loading %s", name);
#endif

    void* data = ScopeStack_load(this->scopeStack, name);
    if(data == NULL) {
        log_logMessage(FATAL, this->name, "Unable to load variable %s.", name);
        component_cleanUpAndStop(this, NULL);
    }
    Stack_push(this->dataStack, data);
    GC_decRef(name);
}

void component_expression(Component_PNTR this, int bytecode_op) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "EXPRESSION %u", bytecode_op);
#endif

    if(Stack_size(this->dataStack) < 2) {
        log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Not enough data in stack", bytecode_op);
        component_cleanUpAndStop(this, NULL);
    }

    TypedObject_PNTR second = Stack_pop(this->dataStack);
    TypedObject_PNTR first = Stack_pop(this->dataStack);

    double castFirst = 0;
    double castSecond = 0;

    if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_REAL) {
        castFirst = *(double*)TypedObject_getObject(first);
    } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_UNSIGNED_INTEGER) {
        castFirst = (double)*(unsigned int*)TypedObject_getObject(first);
    } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_INTEGER) {
        castFirst = (double)*(int*)TypedObject_getObject(first);
    } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_BYTE) {
        castFirst = (double)*(char*)TypedObject_getObject(first);
    }

    if(TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_REAL) {
        castSecond = *(double*)TypedObject_getObject(second);
    } else if(TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_UNSIGNED_INTEGER) {
        castSecond = (double)*(unsigned int*)TypedObject_getObject(second);
    } else if(TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_INTEGER) {
        castSecond = (double)*(int*)TypedObject_getObject(second);
    } else if(TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_BYTE) {
        castSecond = (double)*(char*)TypedObject_getObject(second);
    }

    TypedObject_PNTR result = NULL;

    if(bytecode_op == BYTECODE_ADD) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    ADD");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_REAL || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_REAL) {
            result = TypedObject_construct(BYTECODE_TYPE_REAL, GC_alloc(sizeof(double), false));
            *(double*)TypedObject_getObject(result) = castFirst + castSecond;
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_UNSIGNED_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)TypedObject_getObject(result) = (unsigned int)(castFirst + castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)TypedObject_getObject(result) = (int)(castFirst + castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_BYTE || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)TypedObject_getObject(result) = (char)(castFirst + castSecond);
        }
    } else if(bytecode_op == BYTECODE_SUB) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    SUB");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_REAL || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_REAL) {
            result = TypedObject_construct(BYTECODE_TYPE_REAL, GC_alloc(sizeof(double), false));
            *(double*)TypedObject_getObject(result) = castFirst - castSecond;
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_UNSIGNED_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)TypedObject_getObject(result) = (unsigned int)(castFirst - castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)TypedObject_getObject(result) = (int)(castFirst - castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_BYTE || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)TypedObject_getObject(result) = (char)(castFirst - castSecond);
        }
    } else if(bytecode_op == BYTECODE_MUL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    MUL");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_REAL || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_REAL) {
            result = TypedObject_construct(BYTECODE_TYPE_REAL, GC_alloc(sizeof(double), false));
            *(double*)TypedObject_getObject(result) = castFirst * castSecond;
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_UNSIGNED_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)TypedObject_getObject(result) = (unsigned int)(castFirst * castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)TypedObject_getObject(result) = (int)(castFirst * castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_BYTE || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)TypedObject_getObject(result) = (char)(castFirst * castSecond);
        }
    } else if(bytecode_op == BYTECODE_DIV) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    DIV");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_REAL || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_REAL) {
            result = TypedObject_construct(BYTECODE_TYPE_REAL, GC_alloc(sizeof(double), false));
            *(double*)TypedObject_getObject(result) = castFirst / castSecond;
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_UNSIGNED_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)TypedObject_getObject(result) = (unsigned int)(castFirst / castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)TypedObject_getObject(result) = (int)(castFirst / castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_BYTE || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)TypedObject_getObject(result) = (char)(castFirst / castSecond);
        }
    } else if(bytecode_op == BYTECODE_MOD) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    MOD");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_REAL || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_REAL) {
            //Cannot perform % operation on Reals in Insense
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_UNSIGNED_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)TypedObject_getObject(result) = (unsigned int)((int)castFirst % (int)castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_INTEGER || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)TypedObject_getObject(result) = (int)((int)castFirst % (int)castSecond);
        } else if(TypedObject_getTypeByteCode(first) == BYTECODE_TYPE_BYTE || TypedObject_getTypeByteCode(second) == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)TypedObject_getObject(result) = (char)((int)castFirst % (int)castSecond);
        }
    } else if(bytecode_op == BYTECODE_AND) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    AND");
#endif
        if(TypedObject_getTypeByteCode(first) != BYTECODE_TYPE_BOOL || TypedObject_getTypeByteCode(second) != BYTECODE_TYPE_BOOL) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Boolean Operands expected", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)TypedObject_getObject(result) = *(bool*)TypedObject_getObject(first) && *(bool*)TypedObject_getObject(second);
    } else if (bytecode_op == BYTECODE_OR) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    OE");
#endif
        if(TypedObject_getTypeByteCode(first) != BYTECODE_TYPE_BOOL || TypedObject_getTypeByteCode(second) != BYTECODE_TYPE_BOOL) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Boolean Operands expected", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)TypedObject_getObject(result) = *(bool*)TypedObject_getObject(first) || *(bool*)TypedObject_getObject(second);
    } else if(bytecode_op == BYTECODE_LESS || bytecode_op == BYTECODE_LESSEQUAL || bytecode_op == BYTECODE_EQUAL
            || bytecode_op == BYTECODE_MOREEQUAL || bytecode_op == BYTECODE_MORE || bytecode_op == BYTECODE_UNEQUAL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    COMPARISON");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        if(bytecode_op == BYTECODE_LESS) {
            *(bool*)TypedObject_getObject(result) = castFirst < castSecond;
        } else if(bytecode_op == BYTECODE_LESSEQUAL) {
            *(bool*)TypedObject_getObject(result) = castFirst <= castSecond;
        } else if(bytecode_op == BYTECODE_EQUAL) {
            *(bool*)TypedObject_getObject(result) = castFirst == castSecond;
        } else if(bytecode_op == BYTECODE_MOREEQUAL) {
            *(bool*)TypedObject_getObject(result) = castFirst >= castSecond;
        } else if (bytecode_op == BYTECODE_MORE) {
            *(bool*)TypedObject_getObject(result) = castFirst > castSecond;
        } else if(bytecode_op == BYTECODE_UNEQUAL) {
            *(bool*)TypedObject_getObject(result) = castFirst != castSecond;
        }
    }

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Result is %s/%d", *(bool*)TypedObject_getObject(result) ? "TRUE" : "FALSE", *(int*)TypedObject_getObject(result));
#endif
    Stack_push(this->dataStack, result);

    GC_decRef(second);
    GC_decRef(first);
}

void component_not(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "NOT");
#endif

    if(Stack_size(this->dataStack) == 0) {
        log_logMessage(FATAL, this->name, "Syntax error in NOT - Not enough data in stack");
        component_cleanUpAndStop(this, NULL);
    }

    TypedObject_PNTR first = Stack_pop(this->dataStack);
    if(TypedObject_getTypeByteCode(first) != BYTECODE_TYPE_BOOL) {
        log_logMessage(FATAL, this->name, "Syntax error in NOT - Boolean Operand expected");
        component_cleanUpAndStop(this, NULL);
    }

    bool* object = TypedObject_getObject(first);
    *object = !(*object);
    Stack_push(this->dataStack, first);
}

void component_stop(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "STOP");
#endif

    char *name = component_readString(this);
    if(!strcmp(name, "") || !strcmp(name, this->name)) { //INVERT strcmp because 0 = match
        this->stop = true;
    } else {
        TypedObject_PNTR component = ScopeStack_load(this->scopeStack, name);
        if(component == NULL || TypedObject_getTypeByteCode(component) != BYTECODE_TYPE_COMPONENT) {
            log_logMessage(FATAL, this->name, "Component %s could not be found", name);
            component_cleanUpAndStop(this, NULL);
        }
        ((Component_PNTR)TypedObject_getObject(component))->stop = true;
    }
    GC_decRef(name);
}

void component_behaviourJump(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "BEHAVIOUR JUMP");
#endif

    if(this->stop) {
        GC_decRef(component_readData(this));
    } else {
        component_jump(this);
    }
}

void component_jump(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "JUMP");
#endif

    TypedObject_PNTR jumpSize = component_readData(this);
    if(TypedObject_getTypeByteCode(jumpSize) != BYTECODE_TYPE_INTEGER) {
        log_logMessage(FATAL, this->name, "Syntax error - jump must be followed by distance.");
        GC_decRef(jumpSize);
        component_cleanUpAndStop(this, NULL);
    }

    int* jumpBytes = TypedObject_getObject(jumpSize);

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Jumping back %d bytes", *jumpBytes);
#endif

    fseek(this->sourceFile, (*jumpBytes*-1)+1, SEEK_CUR);
    GC_decRef(jumpSize);
}

void component_ifClause(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "IF");
#endif

    TypedObject_PNTR jumpSize = component_readData(this);
    if(TypedObject_getTypeByteCode(jumpSize) != BYTECODE_TYPE_INTEGER) {
        log_logMessage(FATAL, this->name, "Syntax error - if must be followed by skip distance.");
        GC_decRef(jumpSize);
        component_cleanUpAndStop(this, NULL);
    }

    TypedObject_PNTR condition = Stack_pop(this->dataStack);
    if(TypedObject_getTypeByteCode(condition) != BYTECODE_TYPE_BOOL) {
        log_logMessage(FATAL, this->name, "Boolean type expected for if condition.");
        GC_decRef(jumpSize);
        GC_decRef(condition);
        component_cleanUpAndStop(this, NULL);
    }

    int* jumpBytes = TypedObject_getObject(jumpSize);

    if(*(bool*)TypedObject_getObject(condition) == false) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "IF was FALSE, skipping %d bytes", *jumpBytes);
#endif
        fseek(this->sourceFile, *jumpBytes, SEEK_CUR);
        if(fgetc(this->sourceFile) == BYTECODE_ELSE) {
            //Consume the else-jump so that the next byte is the else code.
            GC_decRef(component_readData(this));
        } else {
            //Replace the byte we just read, as it's not an else next.
            fseek(this->sourceFile, -1, SEEK_CUR);
        }
    } else {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "IF was TRUE, not skipping %d bytes", *jumpBytes);
#endif
    }

    GC_decRef(jumpSize);
    GC_decRef(condition);
}

void component_elseClause(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "ELSE");
#endif
    TypedObject_PNTR jumpSize = component_readData(this);
    if(TypedObject_getTypeByteCode(jumpSize) != BYTECODE_TYPE_INTEGER) {
        log_logMessage(FATAL, this->name, "Syntax error - else must be followed by skip distance.");
        GC_decRef(jumpSize);
        component_cleanUpAndStop(this, NULL);
    }

    fseek(this->sourceFile, *(int*)TypedObject_getObject(jumpSize), SEEK_CUR);
    GC_decRef(jumpSize);
}

void component_connect(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "CONNECT");
#endif
    component_load(this);
    TypedObject_PNTR component1 = Stack_pop(this->dataStack);

    if(TypedObject_getTypeByteCode(component1) != BYTECODE_TYPE_COMPONENT) {
        log_logMessage(FATAL, this->name, "Syntax error in CONNECT - expected a component variable name.");
        GC_decRef(component1);
        component_cleanUpAndStop(this, NULL);
    }

    char *name1 = component_readString(this);

    Component_PNTR component1_pntr = TypedObject_getObject(component1);
    while(!component1_pntr->running) {
        //TODO: This is probably not the right way to pause here.
        sleep(1);
    }
    ChannelWrapper_PNTR channel1 = ListMap_get(component1_pntr->channels, name1);

    if(channel1 == NULL) {
        log_logMessage(FATAL, this->name, "Error in CONNECT - channel %s not found", name1);
        component_cleanUpAndStop(this, NULL);
    }

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "  Found channel %s on component %s", name1, component1_pntr->name);
#endif

    component_load(this);
    TypedObject_PNTR component2 = Stack_pop(this->dataStack);

    if(TypedObject_getTypeByteCode(component2) != BYTECODE_TYPE_COMPONENT) {
        log_logMessage(FATAL, this->name, "Syntax error in CONNECT - expected a component variable name.");
        GC_decRef(component1);
        component_cleanUpAndStop(this, NULL);
    }

    char *name2 = component_readString(this);

    Component_PNTR component2_pntr = TypedObject_getObject(component2);
    while(!component2_pntr->running) {
        //Todo: This is probably not the right way to pause here
        sleep(1);
    }

    ChannelWrapper_PNTR channel2 = ListMap_get(component2_pntr->channels, name2);

    if(channel2 == NULL) {
        log_logMessage(FATAL, this->name, "Error in CONNECT - channel %s not found", name2);
        component_cleanUpAndStop(this, NULL);
    }

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "  Found channel %s on component %s", name2, component2_pntr->name);
#endif

    channel_bind(channel1->channel, channel2->channel); //Ordering is unimportant for this function call

#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "  Channel %s and %s connected", name1, name2);
#endif

    GC_decRef(name1);
    GC_decRef(name2);
}

void component_disconnect(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "DISCONNECT");
#endif

    component_load(this);
    TypedObject_PNTR component1 = Stack_pop(this->dataStack);

    if (TypedObject_getTypeByteCode(component1) != BYTECODE_TYPE_COMPONENT) {
        log_logMessage(FATAL, this->name, "Syntax error in DISCONNECT - expected a component variable name.");
        GC_decRef(component1);
        component_cleanUpAndStop(this, NULL);
    }

    char *name1 = component_readString(this);
    ChannelWrapper_PNTR channel1 = ListMap_get(((Component_PNTR)TypedObject_getObject(component1))->channels, name1);

    if (channel1 == NULL) {
        log_logMessage(FATAL, this->name, "Error in DISCONNECT - channel %s not found", name1);
        component_cleanUpAndStop(this, NULL);
    }

    channel_unbind(channel1->channel);

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "  Channel %s disconnected", name1);
#endif

    GC_decRef(name1);
}

void component_send(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "SEND");
#endif

    char *name1 = component_readString(this);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Sending on %s", name1);
#endif
    ChannelWrapper_PNTR channel1 = ListMap_get(this->channels, name1);

    if(channel1 == NULL) {
        log_logMessage(FATAL, this->name, "Error in SEND - couldn't find channel named %s", name1);
        component_cleanUpAndStop(this, NULL);
    }

    TypedObject_PNTR poppedData = Stack_pop(this->dataStack);
    channel_send(channel1->channel, TypedObject_getObject(poppedData), NULL);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Sent object of type %d (loc: %p) on %s", TypedObject_getTypeByteCode(poppedData), TypedObject_getObject(poppedData), name1);
#endif

    //GC_decRef(poppedData);
    GC_decRef(name1);
}

void component_receive(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "RECEIVE");
#endif

    char *name1 = component_readString(this);
    ChannelWrapper_PNTR channel1 = ListMap_get(this->channels, name1);

    TypedObject_PNTR receivedWrapper = TypedObject_construct(channel1->type, NULL);
    void* receivedData = GC_alloc(TypedObject_getSize(channel1->type), false);
    TypedObject_setObject(receivedWrapper, receivedData);

    channel_receive(channel1->channel, receivedData, false);
    Stack_push(this->dataStack, receivedWrapper);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Received object of type %d (loc: %p) on %s", TypedObject_getTypeByteCode(receivedWrapper), TypedObject_getObject(receivedWrapper), name1);
#endif

    GC_decRef(name1);
}

void component_proc(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "PROC DECL");
#endif
    if(this->procs == NULL) {
        this->procs = ListMap_constructor();
    }

    char* procName = component_readString(this);
    Procedure_PNTR newProcedure = Procedure_construct(procName);

    int parameters = fgetc(this->sourceFile);			// NUMBER_OF_PARAMETERS
    log_logMessage(DEBUG, this->name, "    %d params", parameters);
    for( int i = 0; i < parameters; i++ ) {
        //We don't actually care about the type of the parameter (except in debugging).
        // The compiler has already type-checked for us, and a TypedObject will be explicitly constructed to hold
        // the value anyway.
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    Type is %d", fgetc(this->sourceFile));    // TYPE_OF_PARAMETER
#else
        fgetc(this->sourceFile);
#endif
        Procedure_addParameter(newProcedure, component_readString(this));               //NAME_OF_PARAMATER
    }

    //We get the position _after_ reading the params, and add 1, so that when we skip
    // back here, we are at the first line of actual code of this procedure.
    Procedure_setPosition(newProcedure, ftell(this->sourceFile));

    ListMap_declare(this->procs, procName);
    ListMap_put(this->procs, procName, newProcedure);

    component_skipToNext(this, BYTECODE_BLOCKEND);
    //fgetc(this->sourceFile); //Consume the BLOCKEND
}

void component_procCall(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "PROC CALL");
#endif

    char* procName = component_readString(this);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "     %s", procName);
#endif

    Procedure_PNTR proc = NULL;
    if(this->procs != NULL) {
        proc = ListMap_get(this->procs, procName);
    }

    if(proc != NULL) {
        //Program-defined proc

        //Enter a new scope level, and put the return address (i.e. the current next byte index) in
        component_enterScope(this);

        long* fPos = GC_alloc(sizeof(long), false);
        *fPos = ftell(this->sourceFile);

        ScopeStack_declare(this->scopeStack, "_returnAddress");
        ScopeStack_store(this->scopeStack, "_returnAddress", fPos);

        //Then add all the parameters into the scope
        IteratedList_PNTR paramNames = Procedure_getParameters(proc);
        for(unsigned int i = 0; i < IteratedList_getListLength(paramNames); i++) {
            //TODO: Check GC Ref counts of pop/store/getElement values
            char* name = IteratedList_getElementN(paramNames, i);
            ScopeStack_declare(this->scopeStack, name);
            ScopeStack_store(this->scopeStack, name, Stack_pop(this->dataStack));
        }
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "     Seeking to byte %ld", Procedure_getPosition(proc));
#endif
        fseek(this->sourceFile, Procedure_getPosition(proc), SEEK_SET);
    } else {
        if (mainComponent->procs != NULL) {
            //Not in this component, try global
            proc = ListMap_get(mainComponent->procs, procName);
        }
        if (proc != NULL) {
            //In global! Go There.

            //Enter a new scope level, and put the return address (i.e. the current next byte index) in
            component_enterScope(this);

            long *fPos = GC_alloc(sizeof(long), false);
            *fPos = ftell(this->sourceFile);

            char* fPath = getFilePath(Component_getSourceFile(this->name));
            FILE** fLoc = GC_alloc(sizeof(FILE*), false);
            *fLoc = fopen(fPath, "rb");

            ScopeStack_declare(this->scopeStack, "_returnAddress");
            ScopeStack_store(this->scopeStack, "_returnAddress", fPos);
            ScopeStack_declare(this->scopeStack, "_returnSource");
            ScopeStack_store(this->scopeStack, "_returnSource", fLoc);

            //Then add all the parameters into the scope
            IteratedList_PNTR paramNames = Procedure_getParameters(proc);
            for (unsigned int i = 0; i < IteratedList_getListLength(paramNames); i++) {
                //TODO: Check GC Ref counts of pop/store/getElement values
                char *name = IteratedList_getElementN(paramNames, i);
                ScopeStack_declare(this->scopeStack, name);
                ScopeStack_store(this->scopeStack, name, Stack_pop(this->dataStack));
            }
#ifdef DEBUGGINGENABLED
            log_logMessage(DEBUG, this->name, "     Seeking to byte %ld", Procedure_getPosition(proc));
#endif
            this->sourceFile = mainComponent->sourceFile;
            fseek(this->sourceFile, Procedure_getPosition(proc), SEEK_SET);
        } else {
            //Not in this component or global, try std fns
            proc = ListMap_get(standardFunctions, procName);
            if (proc == NULL) {
                log_logMessage(FATAL, this->name, "Procedure %s not found in Component %s or standard functions in %p!"
                        " Terminating.", procName, this->name, standardFunctions);
                component_cleanUpAndStop(this, NULL);
            }

            //Globally defined decl

            IteratedList_PNTR paramNames = Procedure_getParameters(proc);
            unsigned int numParams = IteratedList_getListLength(paramNames);
            void **params = GC_alloc(sizeof(void *) * numParams, false);
            for (unsigned int i = 0; i < numParams; i++) {
                //TODO: Check GC Ref counts of pop/store/getElement values
                params[i] = TypedObject_getObject(Stack_pop(this->dataStack));
            }

            StandardFunction function = (StandardFunction) Procedure_getPosition(proc);
            function(numParams, params);
        }
    }
    GC_decRef(procName);
}

void component_procReturn(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "RETURN");
#endif

    //Get the return address and file (if set) from the scopestack
    long* newPos = ScopeStack_load(this->scopeStack, "_returnAddress");
    FILE** newFile = ScopeStack_load(this->scopeStack, "_returnSource");

    //Jump back to caller
    if(newFile != NULL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    Jumping back to file at %p", newFile);
#endif
        this->sourceFile = *newFile;
    }
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Seeking to byte %ld", *newPos);
#endif
    fseek(this->sourceFile, *newPos, SEEK_SET);

    GC_decRef(newPos);

    //Clear the scope stack for the procedure
    ScopeStack_exitTo(this->scopeStack, "_returnAddress");
    component_exitScope(this);
}

void component_struct(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "STRUCT");
#endif

    int nextByte = fgetc(this->sourceFile);
    switch(nextByte) {
        case BYTECODE_STRUCT_CONSTRUCTOR:
            component_struct_constructor(this);
            break;
        case BYTECODE_STRUCT_LOAD:
            component_struct_load(this);
            break;
        default:
            log_logMessage(FATAL, this->name, "Unknown Byte Read - %u", nextByte);
            component_cleanUpAndStop(this, NULL);
            break;
    }
}

void component_struct_constructor(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    STRUCT CONSTRUCTOR");
#endif

    TypedObject_PNTR newStruct = TypedObject_construct(BYTECODE_TYPE_STRUCT, ListMap_constructor());

    int paramsToRead = fgetc(this->sourceFile);
    if(paramsToRead > 0) {
        ListMap_PNTR paramsList = TypedObject_getObject(newStruct);
        for(int i = 0; i < paramsToRead; i++) {
            fgetc(this->sourceFile); //As in the component_declare method, we don't care about the type here.
            char* name = component_readString(this);
#ifdef DEBUGGINGENABLED
            log_logMessage(DEBUG, this->name, "        Declaring struct field %s", name);
#endif
            ListMap_declare(paramsList, name);
            TypedObject_PNTR object = Stack_pop(this->dataStack);
            log_logMessage(DEBUG, this->name, "        Storing value of type %d in field", TypedObject_getTypeByteCode(object));
            ListMap_put(paramsList, name, object);
        }
    }

    Stack_push(this->dataStack, newStruct);
}

void component_struct_load(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    STRUCT LOAD");
#endif

    char* fieldName = component_readString(this);

    ListMap_PNTR structFields = TypedObject_getObject(Stack_pop(this->dataStack));
    TypedObject_PNTR field = ListMap_get(structFields, fieldName);

    if(field == NULL) {
        log_logMessage(FATAL, this->name, "Field %s does not exist in struct!", fieldName);
        component_cleanUpAndStop(this, NULL);
        return;
    }

    Stack_push(this->dataStack, field);
}

void component_blockEnd(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "BLOCK END");
#endif

    if(this->inProject) {
        //Skip to end of project.
        // We consume one byte - if it's the project end, then nothing left to do.
        // If it's not, then we've consumed the type of the next project block, and can
        // just say skip to next exit.
        int nextByte = fgetc(this->sourceFile);
        if(nextByte != BYTECODE_PROJECT_EXIT) {
            component_skipToNext(this, BYTECODE_PROJECT_EXIT);
        }
        return;
    } else {
        long *returnAddress = ScopeStack_load(this->scopeStack, "_returnAddress");
        if (returnAddress != NULL) {
            //In a called procedure, so implicitly return
            component_procReturn(this);
        } else {
            //Nowhere to return to. Probably a constructor...
            //No-op
        }
    }
}

void component_any(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "ANY CONSTRUCTOR");
#endif

    TypedObject_PNTR typedObject = Stack_pop(this->dataStack);
    TypedObject_PNTR anyObject = TypedObject_construct(BYTECODE_TYPE_ANY, typedObject);
    Stack_push(this->dataStack, anyObject);
}

void component_projectEntry(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "PROJECT ENTRY");
#endif

    component_enterScope(this);
    TypedObject_PNTR anyObject = Stack_pop(this->dataStack);
    TypedObject_PNTR projectedObject = TypedObject_getObject(anyObject);
    char* asName = component_readString(this);
    ScopeStack_declare(this->scopeStack, asName);
    ScopeStack_store(this->scopeStack, asName, projectedObject);

    int nextByte;
    while((nextByte = fgetc(this->sourceFile)) != BYTECODE_PROJECT_EXIT) {
        if(nextByte != TypedObject_getTypeByteCode(projectedObject)) {
            if(nextByte != BYTECODE_TYPE_ANY) {
                //Not the right project, and not the default project.
                // Skip to next project.
                component_skipToNext(this, BYTECODE_BLOCKEND);
                continue;
            }
        }
        //Found the right project. Mark that we're in a project, then let the component continue.
        this->inProject = true;
        return;
    }

    //Run out of projects to try.
    log_logMessage(ERROR, this->name, "No matching project block. Attempting to continue.");

}

void component_projectExit(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "PROJECT EXIT");
#endif

    if(this->inProject == false) {
        log_logMessage(FATAL, this->name, "Read Project Exit while not at end of Project!");
        component_cleanUpAndStop(this, NULL);
        return;
    } else {
        component_exitScope(this);
        this->inProject = false;
    }
}

//------

char* component_readString(Component_PNTR this) {
    int nextByte;

    //Verify there is a string up next
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in string read - expected BYTECODE_TYPE_STRING (6), got %d", nextByte);
        component_cleanUpAndStop(this, NULL);
        return NULL;
    }

    //Get length
    long startPos = ftell(this->sourceFile);
    int numChars = 0;
    bool escapeChar = false;
    while((nextByte = fgetc(this->sourceFile)) != '\0') {
        //TODO: More escape characters
        //Only allowed escaped char (just now) is \n (and \\).
        //Read this char, and if its a \, read the next char to decide what to do.
        //If the next char is not an n, count the \ as a separate character.
        if(nextByte == '\\' && escapeChar == false) {
            escapeChar = true;
            continue;
        }

        if(nextByte != 'n' && nextByte != '\\' && escapeChar == true) {
            numChars += 2;
            escapeChar = false;
        }
        numChars++;
    }
    //Rewind(+1 for \0)
    fseek(this->sourceFile, startPos, SEEK_SET);

    char* string = GC_alloc((size_t) (numChars+1), false);

    numChars = 0;
    escapeChar = false;
    while((nextByte = fgetc(this->sourceFile)) != '\0') {
        if(nextByte == '\\' && escapeChar == false) {
            escapeChar = true;
            continue;
        }

        if(escapeChar == true) {
            if(nextByte == 'n' ) {
                string[numChars] = '\n';
            } else if (nextByte == '\\') {
                string[numChars] = '\\';
            }
            escapeChar = false;
        } else {
            string[numChars] = (char) nextByte;
        }

        numChars++;
    }
    string[numChars] = '\0';

    return string;
}

TypedObject_PNTR component_readData(Component_PNTR this) {
    int type = fgetc(this->sourceFile);
    switch(type) {
        case BYTECODE_TYPE_INTEGER:
            return TypedObject_construct(BYTECODE_TYPE_INTEGER,
                                         component_readNBytes(this, TypedObject_getSize(BYTECODE_TYPE_INTEGER)));
        case BYTECODE_TYPE_UNSIGNED_INTEGER:
            return TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER,
                                         component_readNBytes(this, TypedObject_getSize(BYTECODE_TYPE_UNSIGNED_INTEGER)));
        case BYTECODE_TYPE_REAL:
            return TypedObject_construct(BYTECODE_TYPE_REAL,
                                         component_readNBytes(this, TypedObject_getSize(BYTECODE_TYPE_REAL)));
        case BYTECODE_TYPE_BOOL:
            return TypedObject_construct(BYTECODE_TYPE_BOOL,
                                         component_readNBytes(this, TypedObject_getSize(BYTECODE_TYPE_BOOL)));
        case BYTECODE_TYPE_BYTE:
            return TypedObject_construct(BYTECODE_TYPE_BYTE,
                                         component_readNBytes(this, TypedObject_getSize(BYTECODE_TYPE_BYTE)));
        case BYTECODE_TYPE_STRING:
            fseek(this->sourceFile, -1, SEEK_CUR); //Rewind for the TYPE byte
            return TypedObject_construct(BYTECODE_TYPE_STRING, component_readString(this));
        default:
            log_logMessage(ERROR, this->name, "Unrecognised type - %d", type);
            return NULL;
    }
}

void* component_readNBytes(Component_PNTR this, size_t nBytes) {
    char* result = GC_alloc(nBytes, false);
    for(unsigned int i = 1; i <= nBytes; i++) {
        int nextChar = fgetc(this->sourceFile);
        result[nBytes-i] = (char)nextChar;
    }
    return result;
}

int component_skipToNext(Component_PNTR this, int bytecode) {
    int next = fgetc(this->sourceFile);
    int parameters;
    while( next != bytecode && next != EOF ) {
        switch( next ) {
            case BYTECODE_STOP:
                GC_decRef(component_readString(this));	// COMPONENT_NAME
                break;
            case BYTECODE_PUSH:
                GC_decRef(component_readData(this));    // TYPE, DATA
                break;
            case BYTECODE_DECLARE:
                GC_decRef(component_readString(this));	// VARIABLE_NAME
                component_readNBytes(this, 1);	        // VARIABLE_TYPE
                break;
            case BYTECODE_LOAD:
                GC_decRef(component_readString(this));	// VARIABLE_NAME
                break;
            case BYTECODE_STORE:
                GC_decRef(component_readString(this));	// VARIABLE_NAME
                break;
            case BYTECODE_COMPONENT:
                GC_decRef(component_readString(this));			// COMPONENT_NAME
                parameters = fgetc(this->sourceFile);			// NO_OF_INTERFACE
                for( int i = 0; i < parameters; i++ ) {
                    int channels = fgetc(this->sourceFile);	    // NO_OF_CHANNEL
                    for( int j = 0; j < channels; j++ ) {
                        fgetc(this->sourceFile);				// DIRECTION
                        fgetc(this->sourceFile);				// TYPE
                        GC_decRef(component_readString(this));	// CHANNEL_NAME
                    }
                }
                break;
            case BYTECODE_CALL:
                GC_decRef(component_readString(this));		// COMPONENT_NAME
                parameters = fgetc(this->sourceFile);		// NUMBER_OF_PARAMETERS
                for( int i = 0; i < parameters; i++ )
                    GC_decRef(component_readString(this));  // PARAMETER
                break;
            case BYTECODE_CONSTRUCTOR:
                parameters = fgetc(this->sourceFile);			// NUMBER_OF_PARAMETERS
                for( int i = 0; i < parameters; i++ ) {
                    fgetc(this->sourceFile);					// TYPE_OF_PARAMETER
                    GC_decRef(component_readString(this));		// NAME_OF_PARAMETER
                }
                break;
            case BYTECODE_JUMP:
                GC_decRef(component_readString(this));	// BYTE_JUMP
                break;
            case BYTECODE_IF:
                GC_decRef(component_readString(this));	// BYTE_JUMP
                break;
            case BYTECODE_ELSE:
                GC_decRef(component_readString(this));	// BYTE_JUMP
                break;
            case BYTECODE_CONNECT:
                GC_decRef(component_readString(this));	// COMPONENT_VARIABLE_NAME
                GC_decRef(component_readString(this));	// CHANNEL_NAME
                GC_decRef(component_readString(this));	// COMPONENT_VARIABLE_NAME
                GC_decRef(component_readString(this));	// CHANNEL_NAME
                break;
            case BYTECODE_DISCONNECT:
                GC_decRef(component_readString(this));	// COMPONENT_VARIABLE_NAME
                GC_decRef(component_readString(this));	// CHANNEL_NAME
                break;
            case BYTECODE_SEND:
                GC_decRef(component_readString(this));	// CHANNEL_NAME
                break;
            case BYTECODE_RECEIVE:
                GC_decRef(component_readString(this));	// CHANNEL_NAME
                break;
            case BYTECODE_PROC:
                GC_decRef(component_readString(this));          // PROC_NAME
                parameters = fgetc(this->sourceFile);			// NUMBER_OF_PARAMETERS
                for( int i = 0; i < parameters; i++ ) {
                    fgetc(this->sourceFile);					// TYPE_OF_PARAMETER
                    GC_decRef(component_readString(this));		// NAME_OF_PARAMETER
                }
                break;
            case BYTECODE_PROCCALL:
                GC_decRef(component_readString(this));  // PROC_NAME
                break;
            case BYTECODE_STRUCT:
                switch(fgetc(this->sourceFile)) {
                    case BYTECODE_STRUCT_CONSTRUCTOR:
                        GC_decRef(component_readString(this));          //STRUCT_NAME
                        parameters = fgetc(this->sourceFile);           //NUMBER_OF_PARAMETERS
                        for( int i = 0; i < parameters; i++ ) {
                            fgetc(this->sourceFile);					// TYPE_OF_PARAMETER
                            GC_decRef(component_readString(this));		// NAME_OF_PARAMETER
                        }
                        break;
                    case BYTECODE_STRUCT_LOAD:
                        GC_decRef(component_readString(this));          //FIELD_NAME
                        break;
                    default:
                        break;
                }
                break;
            case BYTECODE_BLOCKEND:
                if(this->inProject == true && bytecode == BYTECODE_PROJECT_EXIT) {
                    int nextChar = fgetc(this->sourceFile);
                    if(nextChar == BYTECODE_PROJECT_EXIT) {
                        //Found char, rewind to let main logic handle.
                        fseek(this->sourceFile, -1, SEEK_CUR);
                    } else {
                        //Not found char. Must be another project block.
                        // Consumed its type already, so let loop continue.
                    }
                }
                break;
			default:
//				ENTERSCOPE, EXITESCOPE,
//				ADD, SUB, MUL, DIV, MOD,
//				LESS, LESSEQUAL, MORE, MOREEQUAL, EQUAL, UNEQUAL,
//				AND, OR, NOT,
//				BITAND, BITOR, BITXOR, BITNOT,
//              RETURN
				break;
        }
        next = fgetc(this->sourceFile);
    }
    return next;
}

char* Component_getSourceFile(char* name) {
    size_t sourceFileNameLength = 8 + strlen(name) + 4 + 1; //"Insense_" + name + ".isc" + '\0'
    char* sourceFile = GC_alloc(sourceFileNameLength, false);
    strcat(sourceFile, "Insense_");
    strcat(sourceFile, name);
    strcat(sourceFile, ".isc");
    sourceFile[sourceFileNameLength-1] = '\0';

    return sourceFile;
}

// decRef function is called when ref count to a Stack object is zero
// before freeing memory for Stack object
static void Component_decRef(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "Decrementing reference to component");
#endif
    log_logMessage(DEBUG, this->name, "   Cleaning Wait Components [1/6]");
    if(this->waitComponents != NULL) {
        GC_decRef(this->waitComponents);
    }
    log_logMessage(DEBUG, this->name, "   Cleaning Parameters [2/6]");
    if(this->parameters != NULL) {
        GC_decRef(this->parameters);
    }
    log_logMessage(DEBUG, this->name, "   Cleaning Scope Stack [3/6]");
    GC_decRef(this->scopeStack);
    log_logMessage(DEBUG, this->name, "   Cleaning Data Stack [4/6]");
    GC_decRef(this->dataStack);
    log_logMessage(DEBUG, this->name, "   Cleaning Channels [5/6]");
    GC_decRef(this->channels);
    log_logMessage(DEBUG, this->name, "   Cleaning Name [6/6]");
    GC_decRef(this->name);
}
