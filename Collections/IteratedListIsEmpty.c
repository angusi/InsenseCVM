
#include "IteratedList.h"      // for linked list

// Returns whether the Collections is empty
bool IteratedList_isEmpty(IteratedList_PNTR l){
    if(l==NULL) {
        return true;
    }
    return(l->first == NULL);
}
