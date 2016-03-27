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

#include "StandardFunctions.h"
#include "../Logger/Logger.h"
#include "../Procedure.h"
#include "../GC/GC_mem.h"

void StandardFunction_init() {
    //TODO: This seems like a really inefficient way to do this...
    if(standardFunctions == NULL) {
        standardFunctions = ListMap_constructor();

        //printString(string s)
        char* name = GC_alloc(strlen("printString")+1, false);
        strcat(name, "printString");
        Procedure_PNTR printString_proc = Procedure_construct(name);
        char* paramName = GC_alloc(strlen("s")+1, false);
        strcat(paramName, "s");
        Procedure_addParameter(printString_proc, paramName);
        Procedure_setPosition(printString_proc, (long)&StandardFunction_printString);
        ListMap_declare(standardFunctions, name);
        ListMap_put(standardFunctions, name, printString_proc);


        //printInt(int i)
        name = GC_alloc(strlen("printInt")+1, false);
        strcat(name, "printInt");
        Procedure_PNTR printInt_proc = Procedure_construct(name);
        paramName = GC_alloc(strlen("i")+1, false);
        strcat(paramName, "i");
        Procedure_addParameter(printInt_proc, paramName);
        Procedure_setPosition(printInt_proc, (long)&StandardFunction_printInt);
        ListMap_declare(standardFunctions, name);
        ListMap_put(standardFunctions, name, printInt_proc);

        //printReal(real r)
        name = GC_alloc(strlen("printReal")+1, false);
        strcat(name, "printReal");
        Procedure_PNTR printReal_proc = Procedure_construct(name);
        paramName = GC_alloc(strlen("r")+1, false);
        strcat(paramName, "r");
        Procedure_addParameter(printReal_proc, paramName);
        Procedure_setPosition(printReal_proc, (long)&StandardFunction_printReal);
        ListMap_declare(standardFunctions, name);
        ListMap_put(standardFunctions, name, printReal_proc);


        printf("Standard Functions initialised at %p\n", (void*)standardFunctions);
    }
}
