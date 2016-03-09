/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include "StandardFunctions.h"
#include "../Logger/Logger.h"
#include <stdio.h>


// Global Print Any function (can be accessed directly from Insense as proc)

void printString_proc(void *this, int argc, void* argv[]){
  // Write output
  if(argc != 1) {
    log_logMessage(ERROR, "printInt_proc", "Unexpected argument count - expected 1, got %d", argc);
    return;
  }

  printf("%s", (char*)argv[0]);
}
