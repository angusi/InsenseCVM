/*
 * Code for declaring/storing and loading variables from a scope stack.
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

#ifndef CVM_HASHLIST_H
#define CVM_HASHLIST_H


#include "IteratedList.h"

typedef struct ListMapEntry ListMapEntry_s, *ListMapEntry_PNTR;
struct ListMapEntry {
    void (*decRef)(ListMapEntry_PNTR pntr);
    char* key;
    void* value;
};

typedef IteratedList_PNTR ListMap_PNTR;
typedef IteratedList_s ListMap_s;

ListMap_PNTR ListMap_constructor();
void ListMap_declare(ListMap_PNTR hashList, char *key);
void*ListMap_get(ListMap_PNTR hashList, char *key);
void ListMap_put(ListMap_PNTR hashList, char *key, void *value);

#endif //CVM_HASHLIST_H
