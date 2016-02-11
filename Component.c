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
#include "Component.h"
#include "BytecodeTable.h"
#include "Main.h"
#include "ChannelWrapper.h"

static void Component_decRef(Component_PNTR pntr);

/**
 * Construct a new component object
 * @param[in] sourceFile String containing path to bytecode source file
 * @param[in] params Iterated List of parameters
 * @return Pointer to new component object
 */
Component_PNTR component_newComponent(char *sourceFile, IteratedList_PNTR params) {
    Component_PNTR this = GC_alloc(sizeof(Component_s), true);
    this->decRef = Component_decRef;

    size_t componentNameSize = strrchr(basename(sourceFile), '.')-basename(sourceFile);
    this->name = GC_alloc(componentNameSize+1, false);
    strncpy(this->name, basename(sourceFile), componentNameSize);

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
        log_logMessage(INFO, this->name, "Waiting on started components.");
        while(Stack_size(this->waitComponents) != 0) {
            Component_PNTR waitOn = ((TypedObject_PNTR)Stack_pop(this->waitComponents))->object;
            log_logMessage(INFO, this->name, "  Waiting on %s (%lu)", waitOn->name, waitOn->threadId);
            pthread_join(waitOn->threadId, NULL);
            GC_decRef(waitOn);
        }
        log_logMessage(INFO, this->name, "All started components stopped.");
    }

    //Copy name so we can use it in the done message, after Component has been trashed.
    char* name = malloc(strlen(this->name)+1);
    strcpy(name, this->name);
    name[strlen(this->name)] = '\0';

    GC_decRef(this);
    log_logMessage(INFO, name, "DONE. Component cleaned up.");
    free(name);
    pthread_exit(__retval);
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
    if(!strcmp(name, this->name)) { //Negated strcmp because 0 is match
        log_logMessage(FATAL, this->name, "Syntax error in COMPONENT - name %d does not match expected %d", name, this->name);
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

    pthread_t newThread;

    size_t sourceFileNameLength = 8 + strlen(name) + 4 + 1; //"Insense_" + name + ".isc" + '\0'
    char* sourceFile = GC_alloc(sourceFileNameLength, false);
    strcat(sourceFile, "Insense_");
    strcat(sourceFile, name);
    strcat(sourceFile, ".isc");
    sourceFile[sourceFileNameLength-1] = '\0';
    char* filePath = getFilePath(sourceFile);
    Component_PNTR newComponent = component_newComponent(filePath, paramsList);
    pthread_create(&newThread, NULL, component_run, newComponent);
    newComponent->threadId = newThread;

    log_logMessage(INFO, this->name, "    Component %s is at address %p", name, newComponent);

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
            if(nextParamType == nextParam->type) {
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

    fgetc(this->sourceFile); //TODO: Check this byte matches TYPE_COMPONENT? (Why?)
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

    TypedObject_PNTR result = NULL;

    if(bytecode_op == BYTECODE_ADD) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    ADD");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(first->type == BYTECODE_TYPE_REAL || second->type == BYTECODE_TYPE_REAL) {
            result = TypedObject_construct(BYTECODE_TYPE_REAL, GC_alloc(sizeof(double), false));
            *(double*)result->object = *(double*)first->object + *(double*)second->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER || second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)result->object = *(unsigned int*)first->object + *(unsigned int*)second->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER || second->type == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)result->object = *(int*)first->object + *(int*)second->object;
        } else if(first->type == BYTECODE_TYPE_BYTE || second->type == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)result->object = *(char*)first->object + *(char*)second->object;
        }
    } else if(bytecode_op == BYTECODE_SUB) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    SUB");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(first->type == BYTECODE_TYPE_REAL || second->type == BYTECODE_TYPE_REAL) {
            result = TypedObject_construct(BYTECODE_TYPE_REAL, GC_alloc(sizeof(double), false));
            *(double*)result->object = *(double*)first->object - *(double*)second->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER || second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)result->object = *(unsigned int*)first->object - *(unsigned int*)second->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER || second->type == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)result->object = *(int*)first->object - *(int*)second->object;
        } else if(first->type == BYTECODE_TYPE_BYTE || second->type == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)result->object = *(char*)first->object - *(char*)second->object;
        }
    } else if(bytecode_op == BYTECODE_MUL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    MUL");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(first->type == BYTECODE_TYPE_REAL || second->type == BYTECODE_TYPE_REAL) {
            result = TypedObject_construct(BYTECODE_TYPE_REAL, GC_alloc(sizeof(double), false));
            *(double*)result->object = *(double*)first->object * *(double*)second->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER || second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)result->object = *(unsigned int*)first->object * *(unsigned int*)second->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER || second->type == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)result->object = *(int*)first->object * *(int*)second->object;
        } else if(first->type == BYTECODE_TYPE_BYTE || second->type == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)result->object = *(char*)first->object * *(char*)second->object;
        }
    } else if(bytecode_op == BYTECODE_DIV) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    DIV");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(first->type == BYTECODE_TYPE_REAL || second->type == BYTECODE_TYPE_REAL) {
            result = TypedObject_construct(BYTECODE_TYPE_REAL, GC_alloc(sizeof(double), false));
            *(double*)result->object = *(double*)first->object / *(double*)second->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER || second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)result->object = *(unsigned int*)first->object / *(unsigned int*)second->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER || second->type == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)result->object = *(int*)first->object / *(int*)second->object;
        } else if(first->type == BYTECODE_TYPE_BYTE || second->type == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)result->object = *(char*)first->object / *(char*)second->object;
        }
    } else if(bytecode_op == BYTECODE_MOD) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    MOD");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }
        if(first->type == BYTECODE_TYPE_REAL || second->type == BYTECODE_TYPE_REAL) {
            //Cannot perform % operation on Reals in Insense
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER || second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, GC_alloc(sizeof(unsigned int), false));
            *(unsigned int*)result->object = *(unsigned int*)first->object % *(unsigned int*)second->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER || second->type == BYTECODE_TYPE_INTEGER) {
            result = TypedObject_construct(BYTECODE_TYPE_INTEGER, GC_alloc(sizeof(int), false));
            *(int*)result->object = *(int*)first->object % *(int*)second->object;
        } else if(first->type == BYTECODE_TYPE_BYTE || second->type == BYTECODE_TYPE_BYTE) {
            result = TypedObject_construct(BYTECODE_TYPE_BYTE, GC_alloc(sizeof(char), false));
            *(char*)result->object = *(char*)first->object % *(char*)second->object;
        }
    } else if(bytecode_op == BYTECODE_AND) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    AND");
#endif
        if(first->type != BYTECODE_TYPE_BOOL || second->type != BYTECODE_TYPE_BOOL) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Boolean Operands expected", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = *(bool*)first->object && *(bool*)second->object;
    } else if (bytecode_op == BYTECODE_OR) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    OE");
#endif
        if(first->type != BYTECODE_TYPE_BOOL || second->type != BYTECODE_TYPE_BOOL) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Boolean Operand expected", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = *(bool*)first->object || *(bool*)second->object;
    } else if(bytecode_op == BYTECODE_LESS) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    LESS");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double castFirst = 0;
        double castSecond = 0;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = *(double*)first->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double)*(unsigned int*)first->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double)*(int*)first->object;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double)*(char*)first->object;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = *(double*)second->object;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double)*(unsigned int*)second->object;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double)*(int*)second->object;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double)*(char*)second->object;
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst < castSecond;
    } else if(bytecode_op == BYTECODE_LESSEQUAL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    LESSEQUAL");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double castFirst = 0;
        double castSecond = 0;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = *(double*)first->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double)*(unsigned int*)first->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double)*(int*)first->object;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double)*(char*)first->object;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = *(double*)second->object;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double)*(unsigned int*)second->object;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double)*(int*)second->object;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double)*(char*)second->object;
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst <= castSecond;
    } else if(bytecode_op == BYTECODE_EQUAL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    EQUAL");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double castFirst = 0;
        double castSecond = 0;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = *(double*)first->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double)*(unsigned int*)first->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double)*(int*)first->object;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double)*(char*)first->object;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = *(double*)second->object;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double)*(unsigned int*)second->object;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double)*(int*)second->object;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double)*(char*)second->object;
        }


        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        log_logMessage(DEBUG, this->name, "    Comparing %f with %f", castFirst, castSecond);
        *(bool*)result->object = castFirst == castSecond;
    }
    else if(bytecode_op == BYTECODE_MOREEQUAL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    MOREEQUAL");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double castFirst = 0;
        double castSecond = 0;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = *(double*)first->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double)*(unsigned int*)first->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double)*(int*)first->object;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double)*(char*)first->object;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = *(double*)second->object;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double)*(unsigned int*)second->object;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double)*(int*)second->object;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double)*(char*)second->object;
        }


        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst >= castSecond;
    } else if(bytecode_op == BYTECODE_MORE) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    MORE");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double castFirst = 0;
        double castSecond = 0;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = *(double*)first->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double)*(unsigned int*)first->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double)*(int*)first->object;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double)*(char*)first->object;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = *(double*)second->object;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double)*(unsigned int*)second->object;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double)*(int*)second->object;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double)*(char*)second->object;
        }


        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst > castSecond;
    } else if(bytecode_op == BYTECODE_UNEQUAL) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "    UNEQUAL");
#endif
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double castFirst = 0;
        double castSecond = 0;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = *(double*)first->object;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double)*(unsigned int*)first->object;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double)*(int*)first->object;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double)*(char*)first->object;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = *(double*)second->object;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double)*(unsigned int*)second->object;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double)*(int*)second->object;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double)*(char*)second->object;
        }


        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst != castSecond;
    }

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Result is %s/%d", *(bool*)result->object ? "TRUE" : "FALSE", *(int*)result->object);
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
    if(first->type != BYTECODE_TYPE_BOOL) {
        log_logMessage(FATAL, this->name, "Syntax error in NOT - Boolean Operand expected");
        component_cleanUpAndStop(this, NULL);
    }

    *((bool*)first->object) = !(*(bool*)first->object);
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
        if(component == NULL || component->type != BYTECODE_TYPE_COMPONENT) {
            log_logMessage(FATAL, this->name, "Component %s could not be found", name);
            component_cleanUpAndStop(this, NULL);
        }
        ((Component_PNTR)component->object)->stop = true;
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
    if(jumpSize->type != BYTECODE_TYPE_INTEGER) {
        log_logMessage(FATAL, this->name, "Syntax error - jump must be followed by distance.");
        GC_decRef(jumpSize);
        component_cleanUpAndStop(this, NULL);
    }

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Jumping back %d bytes", *(int*)jumpSize->object);
#endif

    fseek(this->sourceFile, (*(int*)jumpSize->object*-1)+1, SEEK_CUR);
    GC_decRef(jumpSize);
}

void component_ifClause(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "IF");
#endif

    TypedObject_PNTR jumpSize = component_readData(this);
    if(jumpSize->type != BYTECODE_TYPE_INTEGER) {
        log_logMessage(FATAL, this->name, "Syntax error - if must be followed by skip distance.");
        GC_decRef(jumpSize);
        component_cleanUpAndStop(this, NULL);
    }

    TypedObject_PNTR condition = Stack_pop(this->dataStack);
    if(condition->type != BYTECODE_TYPE_BOOL) {
        log_logMessage(FATAL, this->name, "Boolean type expected for if condition.");
        GC_decRef(jumpSize);
        GC_decRef(condition);
        component_cleanUpAndStop(this, NULL);
    }

    if(*(bool*)condition->object == false) {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "IF was FALSE, skipping %d bytes", *(int*)jumpSize->object);
#endif
        fseek(this->sourceFile, *(int*)jumpSize->object, SEEK_CUR);
        if(fgetc(this->sourceFile) == BYTECODE_ELSE) {
            //Consume the else-jump so that the next byte is the else code.
            GC_decRef(component_readData(this));
        } else {
            //Replace the byte we just read, as it's not an else next.
            fseek(this->sourceFile, -1, SEEK_CUR);
        }
    } else {
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "IF was TRUE, not skipping %d bytes", *(int*)jumpSize->object);
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
    if(jumpSize->type != BYTECODE_TYPE_INTEGER) {
        log_logMessage(FATAL, this->name, "Syntax error - else must be followed by skip distance.");
        GC_decRef(jumpSize);
        component_cleanUpAndStop(this, NULL);
    }

    fseek(this->sourceFile, *(int*)jumpSize->object, SEEK_CUR);
    GC_decRef(jumpSize);
}

void component_connect(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "CONNECT");
#endif
    component_load(this);
    TypedObject_PNTR component1 = Stack_pop(this->dataStack);

    if(component1->type != BYTECODE_TYPE_COMPONENT) {
        log_logMessage(FATAL, this->name, "Syntax error in CONNECT - expected a component variable name.");
        GC_decRef(component1);
        component_cleanUpAndStop(this, NULL);
    }

    char *name1 = component_readString(this);

    while(!((Component_PNTR)component1->object)->running) {
        //TODO: This is probably not the right way to pause here.
        sleep(1);
    }
    ChannelWrapper_PNTR channel1 = ListMap_get(((Component_PNTR)component1->object)->channels, name1);

    if(channel1 == NULL) {
        log_logMessage(FATAL, this->name, "Error in CONNECT - channel %s not found", name1);
        component_cleanUpAndStop(this, NULL);
    }

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "  Found channel %s on component %s", name1, ((Component_PNTR)component1->object)->name);
#endif

    component_load(this);
    TypedObject_PNTR component2 = Stack_pop(this->dataStack);

    if(component2->type != BYTECODE_TYPE_COMPONENT) {
        log_logMessage(FATAL, this->name, "Syntax error in CONNECT - expected a component variable name.");
        GC_decRef(component1);
        component_cleanUpAndStop(this, NULL);
    }

    char *name2 = component_readString(this);

    while(!((Component_PNTR)component2->object)->running) {
        //Todo: This is probably not the right way to pause here
        sleep(1);
    }

    ChannelWrapper_PNTR channel2 = ListMap_get(((Component_PNTR)component2->object)->channels, name2);

    if(channel2 == NULL) {
        log_logMessage(FATAL, this->name, "Error in CONNECT - channel %s not found", name2);
        component_cleanUpAndStop(this, NULL);
    }

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "  Found channel %s on component %s", name2, ((Component_PNTR)component2->object)->name);
#endif

    channel_bind(channel1->channel, channel2->channel); //Ordering is unimportant for this function call

#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "  Channel %s and %s connected", name1, name2);
#endif
}

void component_disconnect(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "DISCONNECT");
#endif

    component_load(this);
    TypedObject_PNTR component1 = Stack_pop(this->dataStack);

    if (component1->type != BYTECODE_TYPE_COMPONENT) {
        log_logMessage(FATAL, this->name, "Syntax error in DISCONNECT - expected a component variable name.");
        GC_decRef(component1);
        component_cleanUpAndStop(this, NULL);
    }

    char *name1 = component_readString(this);
    ChannelWrapper_PNTR channel1 = ListMap_get(((Component_PNTR)component1)->channels, name1);

    if (channel1 == NULL) {
        log_logMessage(FATAL, this->name, "Error in DISCONNECT - channel %s not found", name1);
        component_cleanUpAndStop(this, NULL);
    }

    channel_unbind(channel1->channel);

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "  Channel %s disconnected", name1);
#endif
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
    channel_send(channel1->channel, poppedData->object, NULL);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Sent object of type %d (loc: %p) on %s", poppedData->type, poppedData->object, name1);
#endif
    GC_decRef(poppedData);
}

void component_receive(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "RECEIVE");
#endif

    char *name1 = component_readString(this);
    ChannelWrapper_PNTR channel1 = ListMap_get(this->channels, name1);

    TypedObject_PNTR receivedWrapper = GC_alloc(sizeof(TypedObject_PNTR), true);
    void* receivedData = GC_alloc(TypedObject_getSize(channel1->type), false);
    receivedWrapper->type = channel1->type;
    receivedWrapper->object = receivedData;

    channel_receive(channel1->channel, receivedData, false);
    Stack_push(this->dataStack, receivedWrapper);
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "    Received object of type %d (loc: %p) on %s", receivedWrapper->type, receivedWrapper->object, name1);
#endif
}


//------

char* component_getName(Component_PNTR this) {
    char* name = GC_alloc(strlen(this->name)+1, false);
    strncpy(name, this->name, strlen(this->name));
    name[strlen(this->name)] = '\0';
    return name;
}

char* component_readString(Component_PNTR this) {
    int nextByte;

    //Verify there is a string up next
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in string read - expected BYTECODE_TYPE_STRING (6), got %d", nextByte);
        component_cleanUpAndStop(this, NULL);
        return NULL;
    }

    //Get length
    int numChars = 0;
    while(fgetc(this->sourceFile) != '\0') {
        numChars++;
    }
    //Rewind(+1 for \0)
    fseek(this->sourceFile, (numChars*-1)-1, SEEK_CUR);

    char* string = GC_alloc((size_t) (numChars+1), false);

    numChars = 0;
    while((nextByte = fgetc(this->sourceFile)) != '\0') {
        string[numChars] = (char) nextByte;
        numChars++;
    }
    string[numChars] = '\0';

    return string;
}

TypedObject_PNTR component_readData(Component_PNTR this) {
    int type = fgetc(this->sourceFile);
    switch(type) {
        case BYTECODE_TYPE_INTEGER:
            return TypedObject_construct(BYTECODE_TYPE_INTEGER, component_readNBytes(this, 4));
        case BYTECODE_TYPE_UNSIGNED_INTEGER:
            return TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, component_readNBytes(this, 4));
        case BYTECODE_TYPE_REAL:
            return TypedObject_construct(BYTECODE_TYPE_REAL, component_readNBytes(this, 8));
        case BYTECODE_TYPE_BOOL:
            return TypedObject_construct(BYTECODE_TYPE_BOOL, component_readNBytes(this, 1));
        case BYTECODE_TYPE_BYTE:
            return TypedObject_construct(BYTECODE_TYPE_BYTE, component_readNBytes(this, 1));
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
                GC_decRef(component_readString(this));	// VALUE
                break;
            case BYTECODE_DECLARE:
                GC_decRef(component_readString(this));				// VARIABLE_NAME
                component_readNBytes(this, 1);	// VARIABLE_TYPE
                break;
            case BYTECODE_LOAD:
                GC_decRef(component_readString(this));	// VARIABLE_NAME
                break;
            case BYTECODE_STORE:
                GC_decRef(component_readString(this));	// VARIABLE_NAME
                break;
            case BYTECODE_COMPONENT:
                GC_decRef(component_readString(this));									// COMPONENT_NAME
                parameters = fgetc(this->sourceFile);			// NO_OF_INTERFACE
                for( int i = 0; i < parameters; i++ ) {
                    int channels = fgetc(this->sourceFile);	// NO_OF_CHANNEL
                    for( int j = 0; j < channels; j++ ) {
                        fgetc(this->sourceFile);				// DIRECTION
                        fgetc(this->sourceFile);				// TYPE
                        GC_decRef(component_readString(this));							// CHANNEL_NAME
                    }
                }
                break;
            case BYTECODE_CALL:
                GC_decRef(component_readString(this));								// COMPONENT_NAME
                parameters = fgetc(this->sourceFile);		// NUMBER_OF_PARAMETERS
                for( int i = 0; i < parameters; i++ )
                    GC_decRef(component_readString(this));							// PARAMETER
                break;
            case BYTECODE_CONSTRUCTOR:
                parameters = fgetc(this->sourceFile);			// NUMBER_OF_PARAMETERS
                for( int i = 0; i < parameters; i++ ) {
                    fgetc(this->sourceFile);					// TYPE_OF_PARAMETER
                    GC_decRef(component_readString(this));								// NAME_OF_PARAMETER
                }
                break;
            case BYTECODE_JUMP:
                GC_decRef(component_readString(this));	// BYTE_JUMP
                break;
            case BYTECODE_IF:
                GC_decRef(component_readString(this));	// BYTE_JUMP
                break;
            case BYTECODE_ELSE: // Not sure yet if require?
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
			default:
//				ENTERSCOPE, EXITESCOPE,
//				ADD, SUB, MUL, DIV, MOD,
//				LESS, LESSEQUAL, MORE, MOREEQUAL, EQUAL, UNEQUAL,
//				AND, OR, NOT,
//				BITAND, BITOR, BITXOR, BITNOT
				break;
        }
        next = fgetc(this->sourceFile);
    }
    return next;
}

// decRef function is called when ref count to a Stack object is zero
// before freeing memory for Stack object
static void Component_decRef(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "Decrementing reference to component");
#endif
    log_logMessage(DEBUG, this->name, "   Cleaning Wait Components [1/5]");
    if(this->waitComponents != NULL) {
        GC_decRef(this->waitComponents);
    }
    log_logMessage(DEBUG, this->name, "   Cleaning Parameters [2/5]");
    if(this->parameters != NULL) {
        GC_decRef(this->parameters);
    }
    log_logMessage(DEBUG, this->name, "   Cleaning Scope Stack [3/5]");
    GC_decRef(this->scopeStack);
    log_logMessage(DEBUG, this->name, "   Cleaning Data Stack [4/5]");
    GC_decRef(this->dataStack);
    log_logMessage(DEBUG, this->name, "   Cleaning Name [5/5]");
    GC_decRef(this->name);
}
