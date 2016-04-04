/*
 * Language Strings.
 *
 * Constant strings used in the Garbage Collector for the Insense Virtual Machine.
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

#ifndef GC_STRINGS_H
#define GC_STRINGS_H

#define GARBAGE_COLLECTOR_NAME "Garbage Collector"
#define GARBAGE_COLLECTOR_INITIALISED "GC Subsystem Initialised"
#define GARBAGE_COLLECTOR_OOM "Could not allocate memory (possibly OOM?)"
#define GARBAGE_COLLECTOR_DECREF_NULL "Ignoring call to decrement NULL pointer references"

#ifdef DEBUGGINGENABLED
#define GARBAGE_COLLECTOR_ALLOCATING_BYTES "Allocating %zu bytes memory at pointer %p"
#define GARBAGE_COLLECTOR_INCREFING "Incrementing reference count to %p (currently %d)"
#define GARBAGE_COLLECTOR_DECREFING "Decrementing reference count to %p (currently %d)"
#define GARBAGE_COLLECTOR_FREEING_BYTES "Freeing memory at pointer %p."
#define GARBAGE_COLLECTOR_ASSIGN "Assigning memory at %p to pointer %p."
#endif

#endif //GC_STRINGS_H
