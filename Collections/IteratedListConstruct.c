/*
 * IteratedList.c
 * Implementation of a singly linked list of pointers (void *)
 * which allows insertion (at head), removal of specific 
 * nodes idetified by value, and iterated access to elements in the list
 *
 * Coded to contain void* pointers in case we want to use the list for
 * for a variety of objects.
 *
 * As list node structures are not contained in more than one list at present,
 * memory for nodes is maintained manually and not garbage collected.
 * In contrast, memory for the node payload and list object (containing 
 * pointer to first and next elements in iterated list) is garbage collected.
 * The payload of nodes may be any object for which memory is allocated by
 * DAL_alloc. The payload may be contained by many lists, for example, 
 * a single channel may be referenced by many connections lists.
 * The decRef function for the list is called by DAL_decRef (garbage collector)
 * before freeing memory for the list object when the ref count for a list 
 * object reaches zero. The decRef function simply removes all list nodes,
 * explicitly freeing memory used by list nodes after decrementing the ref 
 * count for all elements referenced by the list nodes.
 *
 * @author jonl
 *
 */

#include "IteratedList.h"      // for linked list
#include "Strings.h"           // Language management
#include "../GC/GC_mem.h"      // for memory management
#include "../Logger/Logger.h"         // for logging

// "private" (static in C-sense) function declarations, not included in header.

// for garbage collection
static void IteratedList_decRef(IteratedList_PNTR pntr);

// End of "private" function declarations


// (public) functions and constructors 
IteratedList_PNTR IteratedList_constructList(){
    IteratedList_PNTR this = (IteratedList_PNTR) GC_alloc(sizeof(IteratedList_s), true);
    if(this == 0){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_CONSTRUCT_LIST_FAILED);
        return 0;
    }
    this->decRef = IteratedList_decRef;
    return(this);
}


// "private" (static in C-sense) functions, not included in header.


// decRef function is called when ref count to a Collections object is zero
// before freeing memory for Collections object
static void IteratedList_decRef(IteratedList_PNTR pntr){
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, ITERATED_LIST_NAME, ITERATED_LIST_DECREF);
#endif
    if(!IteratedList_isEmpty(pntr)) {
        IteratedList_removeAllElements(pntr); // forces decRef on Collections content (node payload)
    }
}

// End of "private" functions