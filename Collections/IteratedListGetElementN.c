
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger/Logger.h"         // for logging

/**
 * Get hold of nth element from the Collections
 *
 * Elements are indexed from 0 to N-1
*/
void* IteratedList_getElementN(IteratedList_PNTR l, unsigned index){
    unsigned count = 0;
    if(l==NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return(NULL);
    }

    if(IteratedList_isEmpty(l)){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_INDEX_OUT_OF_BOUNDS);
        return(NULL);
    }
    IteratedListNode_PNTR current = l->first;
    while (current != NULL  &&  count < index){
        current = current->tail;
        count++;
    }
    if(current == NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_INDEX_OUT_OF_BOUNDS);
        return(NULL);
    }
    return(current->payload);
}
