/*
 * 
 *
 * 
 *
 * Copyright (c) 2016, Angus Ireland
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
#ifndef CVM_GC_MEM_PRIVATE_H
#define CVM_GC_MEM_PRIVATE_H

#include "GC_mem.h"

// mutex to serialise memory operation
extern pthread_mutex_t* GC_mutex;

typedef struct GC_Header {
    unsigned long ref_count; // 64-bit architecture
    bool mem_contains_pointers;
    pthread_mutex_t* mutex;       // pntr to mutex to serialise memory operations
} GC_Header_s, *GC_Header_PNTR;

// Every object crated by the garbage collector must have a function that decrements its reference count
// and garbage collects when at 0 references. It must be pointed to by the first field.
typedef struct GC_Container {
    decRefFunc_t decRef;
} GC_Container_s, *GC_Container_PNTR;

#endif //CVM_GC_MEM_PRIVATE_H
