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

/**
 * Construct a new component object
 * @param[in] sourceFile String containing path to bytecode source file
 * @param[in] params Array of parameters
 * @param[in] paramCount Number of parameters
 * @return Pointer to new component object
 */
Component component_constructor(char* sourceFile, char* params[], int paramCount) {
    Component this = GC_alloc(sizeof(Component_s), true);
    size_t componentNameSize = strrchr(basename(sourceFile), '.')-basename(sourceFile);
    this->name = GC_alloc(componentNameSize, false);
    strncpy(this->name, basename(sourceFile), componentNameSize);
    this->scopeStackBottom = GC_alloc(sizeof(ScopeLevel_PNTR), false);
    this->scopeStackBottom = NULL;

    //TODO: Check file exists
    this->sourceFile = fopen(sourceFile, "rb");

    log_logMessage(INFO, this->name, "Component Created");

    return this;
}

/**
 * Start a component running
 * @param[in,out] component Component instance to run.
 */
void* component_run(void* component) {
    Component this = (Component)component;
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
            case BYTECODE_CALL:
                component_call(this);
                break;
            default:
                log_logMessage(WARNING, this->name, "Unknown Byte Read");
                break;
        }
    }

    log_logMessage(INFO, this->name, "End");


    return NULL;
}



void component_enterScope(Component this) {
    log_logMessage(DEBUG, this->name, "ENTERSCOPE");
    this->scopeStackBottom = scopeStack_enterScope(this->scopeStackBottom);
}

void component_exitScope(Component this) {
    log_logMessage(DEBUG, this->name, "EXITSCOPE");
    this->scopeStackBottom = scopeStack_exitScope(this->scopeStackBottom);
}

Component component_call(Component this) {
    log_logMessage(DEBUG, this->name, "CALL");
    int nextByte;
    if((nextByte = fgetc(this->sourceFile)) != BYTECODE_TYPE_STRING) {
        log_logMessage(FATAL, this->name, "Syntax error in CALL - %d", nextByte);
        //TODO: Exit thread cleanly:
        pthread_exit(NULL);
    } else {
        char* name = component_readString(this);
        int number_of_parameters = fgetc(this->sourceFile);
        log_logMessage(DEBUG, this->name, name);

        //TODO: Parameters for Components

        pthread_t* newThread = GC_alloc(sizeof(pthread_t), false);

        size_t sourceFileNameLength = 8 + strlen(name) + 4 + 1; //"Insense_" + name + ".isc" + '\0'
        char* sourceFile = GC_alloc(sourceFileNameLength, false);
        strcat(sourceFile, "Insense_");
        strcat(sourceFile, name);
        strcat(sourceFile, ".isc");
        sourceFile[sourceFileNameLength] = '\0';
        char* filePath = getFilePath(sourceFile);
        Component newComponent = component_constructor(filePath, NULL, number_of_parameters);
        pthread_create(newThread, NULL, component_run, newComponent);
        newComponent->threadId = newThread;

        //No need to decref newThread, as we've essentially swapped ownership of the reference to the new component.
        GC_decRef(sourceFile);
        GC_decRef(filePath);
        GC_decRef(name);

        return newComponent;
    }
}

char* component_getName(Component this) {
    char* name = GC_alloc(strlen(this->name)+1, false);
    strncpy(name, this->name, strlen(this->name));
    name[strlen(this->name)] = '\0';
    return name;
}



char* component_readString(Component this) {
    int numChars = 0;

    while(fgetc(this->sourceFile) != '\0') {
        numChars++;
    }
    fseek(this->sourceFile, numChars*-1, SEEK_CUR);

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