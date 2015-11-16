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
        //Todo: Exit thread cleanly:
        pthread_exit(NULL);
    }

    this->parameters = params;

    this->dataStack = Stack_constructor();

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
    while((nextByte = fgetc(this->sourceFile)) != EOF) {
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
                component_expression(this, nextByte);
                break;
            default:
                log_logMessage(ERROR, this->name, "Unknown Byte Read - %u", nextByte);
                break;
        }
    }

    log_logMessage(INFO, this->name, "End");

    if(this->waitComponents != NULL) {
        log_logMessage(INFO, this->name, "Waiting on started components.");
        while(Stack_peek(this->waitComponents) != NULL) {
            Component_PNTR waitOn = ((TypedObject_PNTR)Stack_pop(this->waitComponents))->object;
            log_logMessage(INFO, this->name, "  Waiting on %s (%lu)", waitOn->name, waitOn->threadId);
            pthread_join(waitOn->threadId, NULL);
            GC_decRef(waitOn);
        }
        log_logMessage(INFO, this->name, "All started components stopped.");
    }

    return NULL;
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
        //TODO: Exit thread cleanly:
        pthread_exit(NULL);
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
        }
    }
}

Component_PNTR component_call(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "CALL");
#endif
    int nextByte;
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in CALL - %d", nextByte);
        //TODO: Exit thread cleanly:
        pthread_exit(NULL);
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
                    //TODO: Exit thread cleanly:
                    pthread_exit(NULL);
                } else {
                    char *name = component_readString(this);
                    IteratedList_insertElementAtTail(readParams, name);
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
        GC_decRef(this->parameters); //Finished with this list now, can free it up.
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
            //TODO: Exit thread cleanly:
            pthread_exit(NULL);
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
        //TODO: Exit thread cleanly:
        pthread_exit(NULL);
    } else {
        char *name = component_readString(this);
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "   Declaring %s", name);
#endif

        nextByte = fgetc(this->sourceFile);
        ScopeStack_declare(this->scopeStack, name);
    }
}

void component_store(Component_PNTR this) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "STORE");
#endif

    int nextByte;
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in DECLARE - %d", nextByte);
        //TODO: Exit thread cleanly:
        pthread_exit(NULL);
    } else {
        char *name = component_readString(this);
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "   Storing %s", name);
#endif

        ScopeStack_store(this->scopeStack, name, Stack_pop(this->dataStack));
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
        //TODO: Exit thread cleanly:
        pthread_exit(NULL);
    } else {
        char *name = component_readString(this);
#ifdef DEBUGGINGENABLED
        log_logMessage(DEBUG, this->name, "   Loading %s", name);
#endif

        Stack_push(this->dataStack, ScopeStack_load(this->scopeStack, name));
    }
}

void component_expression(Component_PNTR this, unsigned int bytecode_op) {
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, this->name, "EXPRESSION %u", bytecode_op);
#endif

    //TODO: This.

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
                component_readString(this);	// COMPONENT_NAME
                break;
            case BYTECODE_PUSH:
                component_readString(this);	// VALUE
                break;
            case BYTECODE_DECLARE:
                component_readString(this);				// VARIABLE_NAME
                component_readNBytes(this, 1);	// VARIABLE_TYPE
                break;
            case BYTECODE_LOAD:
                component_readString(this);	// VARIABLE_NAME
                break;
            case BYTECODE_STORE:
                component_readString(this);	// VARIABLE_NAME
                break;
            case BYTECODE_COMPONENT:
                component_readString(this);									// COMPONENT_NAME
                parameters = fgetc(this->sourceFile);			// NO_OF_INTERFACE
                for( int i = 0; i < parameters; i++ ) {
                    int channels = fgetc(this->sourceFile);	// NO_OF_CHANNEL
                    for( int j = 0; j < channels; j++ ) {
                        fgetc(this->sourceFile);				// DIRECTION
                        fgetc(this->sourceFile);				// TYPE
                        component_readString(this);							// CHANNEL_NAME
                    }
                }
                break;
            case BYTECODE_CALL:
                component_readString(this);								// COMPONENT_NAME
                parameters = fgetc(this->sourceFile);		// NUMBER_OF_PARAMETERS
                for( int i = 0; i < parameters; i++ )
                    component_readString(this);							// PARAMETER
                break;
            case BYTECODE_CONSTRUCTOR:
                parameters = fgetc(this->sourceFile);			// NUMBER_OF_PARAMETERS
                for( int i = 0; i < parameters; i++ ) {
                    fgetc(this->sourceFile);					// TYPE_OF_PARAMETER
                    component_readString(this);								// NAME_OF_PARAMETER
                }
                break;
            case BYTECODE_JUMP:
                component_readString(this);	// BYTE_JUMP
                break;
            case BYTECODE_IF:
                component_readString(this);	// BYTE_JUMP
                break;
            case BYTECODE_ELSE: // Not sure yet if require?
                component_readString(this);	// BYTE_JUMP
                break;
            case BYTECODE_CONNECT:
                component_readString(this);	// COMPONENT_VARIABLE_NAME
                component_readString(this);	// CHANNEL_NAME
                component_readString(this);	// COMPONENT_VARIABLE_NAME
                component_readString(this);	// CHANNEL_NAME
                break;
            case BYTECODE_DISCONNECT:
                component_readString(this);	// COMPONENT_VARIABLE_NAME
                component_readString(this);	// CHANNEL_NAME
                break;
            case BYTECODE_SEND:
                component_readString(this);	// CHANNEL_NAME
                break;
            case BYTECODE_RECEIVE:
                component_readString(this);	// CHANNEL_NAME
                break;
//			default:
//				ENTERSCOPE, EXITESCOPE,
//				ADD, SUB, MUL, DIV, MOD,
//				LESS, LESSEQUAL, MORE, MOREEQUAL, EQUAL, UNEQUAL,
//				AND, OR, NOT,
//				BITAND, BITOR, BITXOR, BITNOT
//				break;
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
    GC_decRef(this->scopeStack);
    GC_decRef(this->dataStack);
    GC_decRef(this->name);
}