#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger.h"         // for logging


void *IteratedList_getNextElement(IteratedList_PNTR l) {
    if (l == NULL) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
        return (NULL);
    }
    if (IteratedList_isEmpty(l)) {
        log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_INDEX_OUT_OF_BOUNDS);
        return (NULL);
    }

    void *element = l->next->payload;
    l->next = l->next->tail;
    if (l->next == NULL) {
        // if we iterated to the end of the IteratedList
        l->next = l->first; // wrap round to the beginning
    }
    return (element);
}
