
#include "IteratedList.h"      // for linked list

void *IteratedList_searchList(IteratedList_PNTR l, int (*f)(void *element, void *key), void *key){
    void *first, *current;
    first = current = IteratedList_getNextElement(l);
    if(current != NULL){
        do{
            if( f(current, key) == 0)
                return(current);
            current = IteratedList_getNextElement(l);
        } while (current != first);
    }
    return(NULL);
}
