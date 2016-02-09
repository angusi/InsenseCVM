/*
 * Iterated List Strings
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
#ifndef CVM_STRINGS_H
#define CVM_STRINGS_H


static const char* ITERATED_LIST_NAME = "Iterated List";
static const char*ITERATED_LIST_CONSTRUCT_LIST_FAILED = "Could not construct new Iterated List";
static const char* ITERATED_LIST_CONSTRUCT_NODE_FAILED = "Could not construct new Iterated List Node";
static const char* ITERATED_LIST_NULL_POINTER = "List pointer is null";
static const char* ITERATED_LIST_NULL_ELEMENT = "List element is null";
static const char* ITERATED_LIST_INDEX_OUT_OF_BOUNDS = "List index out of bounds or list is empty";
static const char* ITERATED_LIST_ELEMENT_NOT_FOUND = "Element not found";

#ifdef DEBUGGINGENABLED
static const char* ITERATED_LIST_DECREF = "decRef List";
static const char* ITERATED_LIST_NODE_DECREF = "decRef Node";
static const char* ITERATED_LIST_FREEING_NODE = "Freeing node at %p";
#endif

#endif //CVM_STRINGS_H
