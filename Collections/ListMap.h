/*
 * @file ListMap.h
 * Code for declaring/storing and loading variables from a List Map.
 *
 * The key for each pair is a String (char*), the value is a void*, and so can contain any C pointer.
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

#ifndef CVM_LISTMAP_H
#define CVM_LISTMAP_H


#include "IteratedList.h"

typedef struct ListMapEntry ListMapEntry_s, *ListMapEntry_PNTR;
struct ListMapEntry {
    void (*decRef)(ListMapEntry_PNTR pntr);
    char* key;
    void* value;
};

typedef IteratedList_PNTR  ListMap_PNTR;
typedef IteratedList_s ListMap_s;

/**
 * Create a new list map.
 *
 * This method takes no parameters, and returns a pointer to a new ListMap instance.
 */
ListMap_PNTR ListMap_constructor();

/**
 * Declare an item in a ListMap.
 *
 * Items must be "declared" before they can be "put".
 * This registers the key.
 *
 * @param[in,out] listMap The ListMap instance to register the key in
 * @param[in] key The String to register as a new key in the list map
 */
void ListMap_declare(ListMap_PNTR listMap, char *key);

/**
 * Get an item from a ListMap
 *
 * This returns either a pointer to the item, or NULL if the key is not in the map (or the key exists but no value
 * has been "put" there).
 *
 * @param[in] listMap The ListMap to search for the key in
 * @param[in] key The key to search for
 *
 * @return A pointer to some data, or NULL
 */
void*ListMap_get(ListMap_PNTR listMap, char *key);

/**
 * Put an item in the ListMap at the associated key.
 *
 * The key must already have been "declared" in the map.
 *
 * @param[in,out] listMap The ListMap to store the data in
 * @param[in] key The String to store the data with
 * @param[in] value A pointer to the data to store in the map.
 */
bool ListMap_put(ListMap_PNTR listMap, char *key, void *value);

#endif //CVM_LISTMAP_H
