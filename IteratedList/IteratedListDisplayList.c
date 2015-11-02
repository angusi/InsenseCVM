
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger.h"         // for logging
#include <stdio.h>             // for printf

/**
 * Print the contents of a list.
 */
void displayList(IteratedList_PNTR l, void (*f)(void *val)){
    if(l==NULL){
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return;
    }

    IteratedListNode_PNTR current = l->first;
    printf("\nList: [ ");
    while(current!=NULL){
        (*f)(current->payload);
        current = current->tail;
    }
    printf("]\n");
}
