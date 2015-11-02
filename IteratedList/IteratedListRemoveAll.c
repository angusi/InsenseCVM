
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger.h"         // for logging

// removes all elements from a IteratedList l
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
    while( current != NULL){
        previous = current;
        current = current->tail;
        IteratedList_freeNode(previous); // free memory used by IteratedList node
    }
    l->first = l->next = NULL;
}