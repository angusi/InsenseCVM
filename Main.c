/*
 * @file Main.c
 * Insense C Virtual Machine.
 *
 * An implementation in C of a Virtual Machine for Insense Bytecode programs.
 * This project builds on the work of Pitiphol Pholpabu's dissertation for MSc in Networks and Distributed Systems,
 * "A new compiler for Insense", at the University of St Andrews in 2012, as well as the DIAS Insense Project
 * by Jonathan Lewis, Alan Dearle, Pavlo Bazilinskyy, et. al, at the University of St Andrews.
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

#include "Main.h"

#include "Component.h"
#include "Strings.h"

char* directory;

/**
 * Main program entry point.
 *
 *
 */
int main(int argc, char* argv[]) {
    printf("%s %s\n", PROGRAM_NAME, PROGRAM_VERSION);
    log_init();
    GC_init();

    if(argc < 2 || argc == 3 || argc > 4) {
        printf(PROGRAM_USAGE, argv[0]);
        return EXITCODE_INVALID_ARGUMENTS;
    }

    if(argc == 4) {
        log_setLogLevel(argv[3]);
    }

    directory = GC_alloc(strlen(argv[1])+1, false);
    strncpy(directory, argv[1], strlen(argv[1]));

    char* mainFile = getFilePath("Main.isc");
    Component_PNTR mainComponent = component_newComponent(mainFile, NULL);

    pthread_t mainThread;
    pthread_create(&mainThread, NULL, component_run, mainComponent);
    pthread_join(mainThread, NULL);

    GC_decRef(mainFile);
    GC_decRef(directory);

    return EXITCODE_SUCCESS;
}

char* getFilePath(char* fileName) {
    size_t bufferSize = strlen(directory) + strlen(fileName) + 1;
    if(directory[strlen(directory) - 1] != PATH_SEPARATOR) {
        //Make room for the extra slash we need to add
        bufferSize += 1;
    }
    char* mainFile = GC_alloc(bufferSize, false);
    strncpy(mainFile, directory, strlen(directory));
    if(directory[strlen(directory) - 1] != PATH_SEPARATOR) {
        mainFile[strlen(directory)] = PATH_SEPARATOR;
    }
    strcat(mainFile, fileName);
    mainFile[bufferSize-1] = '\0';

    return mainFile;
}