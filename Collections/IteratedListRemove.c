
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger/Logger.h"         // for logging
#include "../GC/GC_mem.h"


// Removes an element from the List l, decrements ref count on element
void IteratedList_removeElement(IteratedList_PNTR l, void *element) {
    // case: error, l is null, i.e. no List defined
    if(l==NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return;
    }
    if(IteratedList_isEmpty(l)){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_ELEMENT_NOT_FOUND);
        return;
    }
    if(element == NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_ELEMENT_NOT_FOUND);
        return;
    }

    // case: element we want to remove is right at the start of the List
    if(l->first->payload == element){
        IteratedListNode_PNTR oldFirst = l->first;
        l->first = oldFirst->tail;
        // take action if we are removing the next iterator node
        if(l->next == oldFirst) l->next = l->first;
        GC_decRef(oldFirst);
        return;
    }
    // case: otherwise try to find element to remove
    IteratedListNode_PNTR previous = l->first, current = previous->tail;
    while(current != NULL && current->payload!=element){
        previous = current;
        current = current->tail;
    }
    if(current!=NULL){ // we found the element to remove
        // take action if we are removing the next iterator node
        if(l->next == current) l->next = current->tail;
        if(l->next == NULL) l->next = l->first;
        // deal with removal
        previous->tail = current->tail;
        // explicitly free memory used by node
        GC_decRef(current);
    }
    else {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_ELEMENT_NOT_FOUND);
    }
}
