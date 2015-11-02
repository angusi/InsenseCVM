#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger.h"         // for logging
#include "../GC/GC_mem.h"      // for memory management

static IteratedListNode_PNTR IteratedList_constructNode() {
    // Not maintaining IteratedList nodes using garbage collector, waste of space and
    // time as you always know when a node has been created and removed from the
    // IteratedList, so using BASE_mem_alloc here and will use explicit BASE_mem_free
    // in IteratedList_freeNode function (declared above & defined at bottom of file)
    //IteratedListNode_PNTR this=(IteratedListNode_PNTR) DAL_alloc(sizeof(IteratedListNode_s),false);
    IteratedListNode_PNTR this = (IteratedListNode_PNTR) malloc(sizeof(IteratedListNode_s));
    if (this == 0) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_CONSTRUCT_NODE_FAILED);
        return 0;
    }
    // need set to NULL here as using malloc which does not set mem to 0
    this->payload = NULL;
    this->tail = NULL;
    return this;
}


void IteratedList_insertElement(IteratedList_PNTR l, void *element) {
    bool emptyList;
    emptyList = IteratedList_isEmpty(l);
    if (l == NULL) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return;
    }
    // putting NULL in the IteratedList is an issue if the IteratedList is supposed
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
    if (emptyList) { // we are about to add the first element to this IteratedList set the
        l->next = l->first; // iterator index to point to the first node
    }
} 