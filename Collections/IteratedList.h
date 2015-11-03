/**
 * IteratedList.h
 * Header file for singly linked iterated list implementation.
 *
 * @author jonl
 *
*/

#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#ifndef LIST_H_
#define LIST_H_

typedef struct IteratedListNode IteratedListNode_s, *IteratedListNode_PNTR;
struct IteratedListNode {
    void (*decRef)(IteratedListNode_PNTR pntr);
    void *payload;
    IteratedListNode_PNTR tail;
};

typedef struct IteratedList IteratedList_s, *IteratedList_PNTR;
struct IteratedList {
    void (*decRef)(IteratedList_PNTR pntr);
    IteratedListNode_PNTR first;   // the first node in the Collections, NULL if Collections is empty
    IteratedListNode_PNTR next;    // used for iterating over lists using IteratedList_getNextElement
};

// constructor
extern IteratedList_PNTR IteratedList_constructList();

// functions to insert into, get and remove elements from a Collections
extern void IteratedList_insertElement(IteratedList_PNTR l, void *element);

extern void *IteratedList_getNextElement(IteratedList_PNTR l);
extern void *IteratedList_getElementN(IteratedList_PNTR l, unsigned index);

extern unsigned IteratedList_getListLength(IteratedList_PNTR l);

extern bool IteratedList_containsElement(IteratedList_PNTR l, void *element);
extern bool IteratedList_isEmpty(IteratedList_PNTR l);
extern void *IteratedList_searchList(IteratedList_PNTR l, int (*f)(void *element, void *key), void *key);

extern void IteratedList_removeElement(IteratedList_PNTR l, void *element);
extern void IteratedList_removeAllElements(IteratedList_PNTR l);

#endif
