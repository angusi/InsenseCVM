
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger/Logger.h"         // for logging


unsigned IteratedList_getListLength(IteratedList_PNTR l){
  unsigned count = 0;
  if(l==NULL){
    log_logMessage(ERROR, ITERATED_LIST_NAME, ITERATED_LIST_NULL_POINTER);
    return 0;
  }
  IteratedListNode_PNTR current = l->first;
  while (current != NULL){
    current = current->tail;
    count++;
  }
  return(count);
}
