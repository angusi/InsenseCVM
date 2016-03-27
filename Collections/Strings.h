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


#define ITERATED_LIST_NAME "Iterated List"
#define ITERATED_LIST_CONSTRUCT_LIST_FAILED "Could not construct new Iterated List"
#define ITERATED_LIST_CONSTRUCT_NODE_FAILED "Could not construct new Iterated List Node"
#define ITERATED_LIST_NULL_POINTER "List pointer is null"
#define ITERATED_LIST_NULL_ELEMENT "List element is null"
#define ITERATED_LIST_INDEX_OUT_OF_BOUNDS "List index out of bounds or list is empty"
#define ITERATED_LIST_ELEMENT_NOT_FOUND "Element not found"
#define ITERATED_LIST_DISPLAY_PREFIX "\nList: [ "
#define ITERATED_LIST_DISPLAY_SUFFIX "]\n"

#define STACK_NAME "Stack"
#define STACK_CONSTRUCT_FAILED "Failed to construct Stack"
#define STACK_UNDERFLOW "Underflow: Cannot pop from empty stack"
#define STACK_PEEK_UNDERFLOW "Underflow: Cannot peek at empty stack"

#ifdef DEBUGGINGENABLED
#define ITERATED_LIST_DECREF "decRef List"
#define ITERATED_LIST_NODE_DECREF "decRef Node"
#define ITERATED_LIST_FREEING_NODE "Freeing node at %p"

#define LISTMAP_NAME "ListMap"
#define LISTMAP_PUT_UNKNOWN_KEY "Tried to put data in unknown key %s"

#define STACK_CONSTRUCT "Constructing Stack"
#define STACK_PUSH "Pushing %p onto stack at %p"
#define STACK_POP "Popping from stack at %p"
#define STACK_PEEK "Peeking at stack at %p"
#endif

#endif //CVM_STRINGS_H
