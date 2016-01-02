
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger/Logger.h"         // for logging
#include "../GC/GC_mem.h"

// removes all elements from a Collections l
// by assigning null to each node payload the ref count for 
// the payload is decremented

void IteratedList_removeAllElements(IteratedList_PNTR l){
    if(l==NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return;
    }
    if(IteratedList_isEmpty(l)){
        return;
    }
    IteratedListNode_PNTR previous, current = l->first;
    l->last->tail = NULL;
    while( current != NULL){
        previous = current;
        current = current->tail;
        GC_decRef(previous); // free memory used by Collections node
    }
    l->first = l->next = NULL;
}