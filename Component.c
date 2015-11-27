/*
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

#include "Component.h"
#include "BytecodeTable.h"
#include "Main.h"
#include "TypedObject.h"

static void Component_decRef(Component_PNTR pntr);

/**
 * Construct a new component object
 * @param[in] sourceFile String containing path to bytecode source file
 * @param[in] params Array of parameters
 * @param[in] paramCount Number of parameters
 * @return Pointer to new component object
 */
Component_PNTR component_newComponent(char *sourceFile, IteratedList_PNTR params, int paramCount) {
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

    this->stop = false;

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
    log_logMessage(DEBUG, this->name, "    %d interfaces", number_of_interfaces);
    for(int i = 0; i < number_of_interfaces; i++) {
        int number_of_channels = fgetc(this->sourceFile);
        log_logMessage(DEBUG, this->name, "    %d channels", number_of_channels);

        for(int j = 0; j < number_of_channels; j++) {
            int channel_direction = fgetc(this->sourceFile);
            int channel_type = fgetc(this->sourceFile);
            char* channel_name = component_readString(this);
            //TODO: channels
            GC_decRef(channel_name);
        }
    }
    GC_decRef(name);
}

Component_PNTR component_call(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "CALL");
#endif
    int nextByte;
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in CALL - %d", nextByte);
        component_cleanUpAndStop(this, NULL);
        return NULL; //NULL - cleanUp will have cleaned up and terminated thread already.
    } else {
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
        Component_PNTR newComponent = component_newComponent(filePath, paramsList, number_of_parameters);
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
    if(paramsToRead == givenParameters) {
        readParams = IteratedList_constructList();
        for(int i = 0; i < givenParameters; i++) {
            int nextParamType = fgetc(this->sourceFile);
            TypedObject_PNTR nextParam = IteratedList_getNextElement(this->parameters);
            if(nextParamType == nextParam->type) {
                int nextByte;
                if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
                    log_logMessage(FATAL, this->name, "Syntax error in CONSTRUCTOR - %d", nextByte);
                    component_cleanUpAndStop(this, NULL);
                } else {
                    char *name = component_readString(this);
                    IteratedList_insertElementAtTail(readParams, name);
                    GC_decRef(name);
                }
            } else {
                thisConstructor = false;
                GC_decRef(component_readString(this)); //Immediately discard data
            }
        }
    } else {
        thisConstructor = false;
        for(int i = 0; i < givenParameters; i++) {
            fgetc(this->sourceFile); //Skip type
            GC_decRef(component_readString(this)); //Skip name
        }
    }

    if(thisConstructor) {
        log_logMessage(INFO, this->name, "  Constructor match");
        for(int i = 0; i < paramsToRead; i++) {
            IteratedList_rewind(readParams);
            IteratedList_rewind(this->parameters);
            char* name = IteratedList_getNextElement(readParams);
            ScopeStack_declare(this->scopeStack, name);
            ScopeStack_store(this->scopeStack, name, IteratedList_getNextElement(this->parameters));
            GC_decRef(name);
        }

        //Finished with these lists now, can free them up.
        if(this->parameters != NULL) {
            GC_decRef(this->parameters);
        }
        GC_decRef(readParams);
    } else {
        log_logMessage(INFO, this->name, " Constructor mismatch, fastforwarding");

        GC_decRef(readParams);
        IteratedList_rewind(this->parameters);

        int nextByte = component_skipToNext(this, BYTECODE_CONSTRUCTOR);
        if( nextByte == BYTECODE_CONSTRUCTOR ) {
            component_constructor(this);
        } else {
            log_logMessage(FATAL, this->name, "Constructor not fond!");
            component_cleanUpAndStop(this, NULL);
        }
    }
}

void component_declare(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "DECLARE");
#endif
    int nextByte;
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in DECLARE - %d", nextByte);
        component_cleanUpAndStop(this, NULL);
    } else {
        char* name = component_readString(this);
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "   Declaring %s", name);
#endif

        fgetc(this->sourceFile); //TODO: Check this byte matches TYPE_COMPONENT?
        ScopeStack_declare(this->scopeStack, name);
        GC_decRef(name);
    }
}

void component_store(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "STORE");
#endif

    int nextByte;
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in DECLARE - %d", nextByte);
        component_cleanUpAndStop(this, NULL);
    } else {
        char *name = component_readString(this);
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "   Storing %s", name);
#endif

        ScopeStack_store(this->scopeStack, name, Stack_pop(this->dataStack));
        GC_decRef(name);
    }
}

void component_push(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "PUSH");
#endif

    int type = fgetc(this->sourceFile);
    char* string; //In case we have a string
    switch(type) {
        case BYTECODE_TYPE_INTEGER:
            Stack_push(this->dataStack, TypedObject_construct(BYTECODE_TYPE_INTEGER, component_readNBytes(this, 4)));
            break;
        case BYTECODE_TYPE_UNSIGNED_INTEGER:
            Stack_push(this->dataStack, TypedObject_construct(BYTECODE_TYPE_UNSIGNED_INTEGER, component_readNBytes(this, 4)));
            break;
        case BYTECODE_TYPE_REAL:
            Stack_push(this->dataStack, TypedObject_construct(BYTECODE_TYPE_REAL, component_readNBytes(this, 8)));
            break;
        case BYTECODE_TYPE_BOOL:
            Stack_push(this->dataStack, TypedObject_construct(BYTECODE_TYPE_BOOL, component_readNBytes(this, 1)));
            break;
        case BYTECODE_TYPE_BYTE:
            Stack_push(this->dataStack, TypedObject_construct(BYTECODE_TYPE_BYTE, component_readNBytes(this, 1)));
            break;
        case BYTECODE_TYPE_STRING:
            string = component_readString(this);
            Stack_push(this->dataStack, TypedObject_construct(BYTECODE_TYPE_STRING, string));
            GC_decRef(string);
            break;
        default:
            log_logMessage(ERROR, this->name, "Unrecognised type - %d", type);
    }
}


void component_load(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "LOAD");
#endif

    int nextByte;
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in LOAD - %d", nextByte);
        component_cleanUpAndStop(this, NULL);
    } else {
        char *name = component_readString(this);
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "   Loading %s", name);
#endif

        Stack_push(this->dataStack, ScopeStack_load(this->scopeStack, name));
        GC_decRef(name);
    }
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
        if(first->type != BYTECODE_TYPE_BOOL || second->type != BYTECODE_TYPE_BOOL) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Boolean Operands expected", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = *(bool*)first->object && *(bool*)second->object;
    } else if (bytecode_op == BYTECODE_OR) {
        if(first->type != BYTECODE_TYPE_BOOL || second->type != BYTECODE_TYPE_BOOL) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Boolean Operand expected", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = *(bool*)first->object || *(bool*)second->object;
    } else if(bytecode_op == BYTECODE_LESS) {
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double* castFirst = NULL;
        double* castSecond = NULL;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = (double*)first->type;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double *) (unsigned int*)first->type;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double *) (int*)first->type;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double *) (char*)first->type;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = (double*)second->type;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double *) (unsigned int*)second->type;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double *) (int*)second->type;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double *) (char*)second->type;
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst < castSecond;
    } else if(bytecode_op == BYTECODE_LESSEQUAL) {
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double* castFirst = NULL;
        double* castSecond = NULL;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = (double*)first->type;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double *) (unsigned int*)first->type;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double *) (int*)first->type;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double *) (char*)first->type;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = (double*)second->type;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double *) (unsigned int*)second->type;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double *) (int*)second->type;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double *) (char*)second->type;
        }

        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst <= castSecond;
    } else if(bytecode_op == BYTECODE_EQUAL) {
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double* castFirst = NULL;
        double* castSecond = NULL;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = (double*)first->type;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double *) (unsigned int*)first->type;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double *) (int*)first->type;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double *) (char*)first->type;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = (double*)second->type;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double *) (unsigned int*)second->type;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double *) (int*)second->type;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double *) (char*)second->type;
        }


        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst == castSecond;
    }
    else if(bytecode_op == BYTECODE_MOREEQUAL) {
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double* castFirst = NULL;
        double* castSecond = NULL;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = (double*)first->type;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double *) (unsigned int*)first->type;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double *) (int*)first->type;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double *) (char*)first->type;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = (double*)second->type;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double *) (unsigned int*)second->type;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double *) (int*)second->type;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double *) (char*)second->type;
        }


        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst >= castSecond;
    } else if(bytecode_op == BYTECODE_MORE) {
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double* castFirst = NULL;
        double* castSecond = NULL;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = (double*)first->type;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double *) (unsigned int*)first->type;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double *) (int*)first->type;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double *) (char*)first->type;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = (double*)second->type;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double *) (unsigned int*)second->type;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double *) (int*)second->type;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double *) (char*)second->type;
        }


        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst > castSecond;
    } else if(bytecode_op == BYTECODE_UNEQUAL) {
        if(!TypedObject_isNumber(first) || !TypedObject_isNumber(second)) {
            log_logMessage(FATAL, this->name, "Syntax error in EXPR %u - Operand Type Mismatched", bytecode_op);
            component_cleanUpAndStop(this, NULL);
        }

        double* castFirst = NULL;
        double* castSecond = NULL;

        if(first->type == BYTECODE_TYPE_REAL) {
            castFirst = (double*)first->type;
        } else if(first->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castFirst = (double *) (unsigned int*)first->type;
        } else if(first->type == BYTECODE_TYPE_INTEGER) {
            castFirst = (double *) (int*)first->type;
        } else if(first->type == BYTECODE_TYPE_BYTE) {
            castFirst = (double *) (char*)first->type;
        }

        if(second->type == BYTECODE_TYPE_REAL) {
            castSecond = (double*)second->type;
        } else if(second->type == BYTECODE_TYPE_UNSIGNED_INTEGER) {
            castSecond = (double *) (unsigned int*)second->type;
        } else if(second->type == BYTECODE_TYPE_INTEGER) {
            castSecond = (double *) (int*)second->type;
        } else if(second->type == BYTECODE_TYPE_BYTE) {
            castSecond = (double *) (char*)second->type;
        }


        result = TypedObject_construct(BYTECODE_TYPE_BOOL, GC_alloc(sizeof(bool), false));
        *(bool*)result->object = castFirst != castSecond;
    }

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

    int nextByte;
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in STOP - %d", nextByte);
        component_cleanUpAndStop(this, NULL);
    } else {
        char *name = component_readString(this);
        if(!strcmp(name, "") || !strcmp(name, this->name)) { //INVERT strcmp because 0 = match
            this->stop = true;
        } else {
            TypedObject_PNTR component = ScopeStack_load(this->scopeStack, name);
            if(component == NULL || component->type != BYTECODE_TYPE_COMPONENT) {
                log_logMessage(FATAL, this->name, "Component %s could not be found", name, nextByte);
                component_cleanUpAndStop(this, NULL);
            }
            ((Component_PNTR)component->object)->stop = true;
        }
        GC_decRef(name);
    }

}

//------

char* component_getName(Component_PNTR this) {
    char* name = GC_alloc(strlen(this->name)+1, false);
    strncpy(name, this->name, strlen(this->name));
    name[strlen(this->name)] = '\0';
    return name;
}

char* component_readString(Component_PNTR this) {
    int numChars = 0;

    //Get length
    while(fgetc(this->sourceFile) != '\0') {
        numChars++;
    }
    //Rewind(+1 for \0)
    fseek(this->sourceFile, (numChars*-1)-1, SEEK_CUR);

    char* string = GC_alloc((size_t) (numChars+1), false);
    int nextByte;
    numChars = 0;
    while((nextByte = fgetc(this->sourceFile)) != '\0') {
        string[numChars] = (char) nextByte;
        numChars++;
    }
    string[numChars] = '\0';

    return string;
}

void* component_readNBytes(Component_PNTR this, size_t nBytes) {
    char* result = GC_alloc(nBytes, false);
    for(unsigned int i = 0; i < nBytes; i++) {
        int nextChar = fgetc(this->sourceFile);
        result[i] = (char)nextChar;
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