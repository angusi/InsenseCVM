
#include "IteratedList.h"      // for linked list
#include "Strings.h"           // for Language management
#include "../Logger.h"         // for logging
#include "../GC/GC_mem.h"      // for memory management

// Explicit free for IteratedList node structures .
// As IteratedList nodes cannot be shared among different lists at present, memory
// for nodes is maintained manually and not garbage collected.
// In contrast, memory for IteratedList payload is garbage collected, may have same
// item in many lists ... so IteratedList_freeNode calls GC_decRef on these objects

void IteratedList_freeNode(IteratedListNode_PNTR node){
  // force ref count decrement on object being kept in node payload
  GC_decRef(node->payload);
  free(node);
}

