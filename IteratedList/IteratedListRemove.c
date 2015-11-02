
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger.h"         // for logging


// Removes an element from the IteratedList l, decrements ref count on element
void IteratedList_removeElement(IteratedList_PNTR l, void *element) {
    // case: error, l is null, i.e. no IteratedList defined
    if(l==NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return;
    }
    if(IteratedList_isEmpty(l)){
        return;
    }
    if(element == NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_ELEMENT_NOT_FOUND);
        return;
    }

    // case: element we want to remove is right at the start of the IteratedList
    if(l->first->payload == element){
        IteratedListNode_PNTR oldFirst = l->first;
        l->first = oldFirst->tail;
        // take action if we are removing the next iterator node
        if(l->next == oldFirst) l->next = l->first;
        IteratedList_freeNode(oldFirst); // explicitly free memory used by node
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
        IteratedList_freeNode(current);
    }
    else {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_ELEMENT_NOT_FOUND);
    }
}
