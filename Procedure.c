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

#include "Procedure.h"
#include "GC/GC_mem.h"
#include "Logger/Logger.h"

struct Procedure {
    void (*decRef)(Procedure_PNTR procedure);
    char* name;
    long position;
    IteratedList_PNTR parameterNames;
};

static void Procedure_decRef(Procedure_PNTR pntr);

Procedure_PNTR Procedure_construct(char* name) {
    Procedure_PNTR newProcedure = GC_alloc(sizeof(Procedure_s), true);
    newProcedure->name = name;
    newProcedure->parameterNames = IteratedList_constructList();
    newProcedure->decRef = Procedure_decRef;
    return newProcedure;
}

void Procedure_addParameter(Procedure_PNTR this, char* name) {
    IteratedList_insertElement(this->parameterNames, name);
}

IteratedList_PNTR Procedure_getParameters(Procedure_PNTR this) {
    return this->parameterNames;
}

void Procedure_setPosition(Procedure_PNTR this, long position) {
    this->position = position;
}

long Procedure_getPosition(Procedure_PNTR this) {
    if(this->position == 0) {
        log_logMessage(FATAL, this->name, "Procedure Position never defined");
        return 0;
    }

    return this->position;
}

void Procedure_decRef(Procedure_PNTR this) {
    GC_decRef(this->name);
    GC_decRef(this->parameterNames);
}