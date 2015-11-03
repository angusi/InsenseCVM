
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger/Logger.h"         // for logging

/**
 * Check if a given element is in a list.
 */
bool IteratedList_containsElement(IteratedList_PNTR l, void *element){
    if(l==NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return(false);
    }

    if(IteratedList_isEmpty(l)){
        return(false);
    }

    IteratedListNode_PNTR current = l->first;
    while (current != NULL){
        if(current->payload == element)
            return(true);
        current = current->tail;
    }

    return(false);
}
