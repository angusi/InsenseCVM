/*
 * @file GC_mem.h
 *
 * Memory management declarations.
 *
 *
 *
 * Copyright (c) 2015, Angus Ireland
 * School of Computer Science, St. Andrews University
 *
 * A simplified version of memory management function declarations from the Insense Runtime for Unix.
 *
 *
 * Copyright (c) 2013, Jonathan Lewis, Alan Dearle, Pavlo Bazilinskyy <pavlo.bazilinskyy@gmail.com>
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

#ifndef GC_MEM_H
#define GC_MEM_H

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef void (*decRefFunc_t)(void* pntr);

extern void GC_init();
extern void GC_assign(void *generic_var_pntr, void *new_mem);
extern void* GC_alloc(size_t size, bool contains_pointers);
//extern unsigned GC_getRef(void* pntr);
extern bool GC_mem_contains_pointers(void* pntr);
//extern void GC_mem_set_contains_pointers(void* pntr, bool mem_contains_pointers);
extern void GC_decRef(void* pntr);
extern void GC_incRef(void* pntr);

#endif /*GC_MEM_H*/
