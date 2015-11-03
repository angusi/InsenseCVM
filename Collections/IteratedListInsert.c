#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger/Logger.h"         // for logging
#include "../GC/GC_mem.h"      // for memory management

// for garbage collection
static void IteratedListNode_decRef(IteratedListNode_PNTR pntr);

static IteratedListNode_PNTR IteratedList_constructNode() {
    IteratedListNode_PNTR this = (IteratedListNode_PNTR) GC_alloc(sizeof(IteratedListNode_s), true);
    if (this == 0) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_CONSTRUCT_NODE_FAILED);
        return 0;
    }
    this->decRef = IteratedListNode_decRef;
    this->payload = NULL;
    this->tail = NULL;
    return this;
}


void IteratedList_insertElement(IteratedList_PNTR l, void *element) {

    if (l == NULL) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return;
    }

    // putting NULL in the Collections is an issue if the Collections is supposed
    // to contain valid references to objects, so prevented here
    if (element == NULL) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_ELEMENT);
        return;
    }

    IteratedListNode_PNTR newNode = IteratedList_constructNode();
    if (newNode == NULL) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_CONSTRUCT_NODE_FAILED);
        return;
    }
    GC_assign(&newNode->payload, element); // increases ref count on element

    newNode->tail = l->first;
    l->first = newNode;

    if (IteratedList_isEmpty(l)) { // we are about to add the first element to this Collections
        l->next = l->first; // set the iterator index to point to the first node
    }
}

// Called when there are no more references to this node.
static void IteratedListNode_decRef(IteratedListNode_PNTR pntr){
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, ITERATED_LIST_NAME, ITERATED_LIST_NODE_DECREF);
#endif

    if(pntr == NULL) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_ELEMENT);
        return;
    }

    GC_decRef(pntr->payload);
}