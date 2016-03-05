/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include "StandardFunctions.h"
#include "DAL_mem.h"
#include "events.h"
#include <stdio.h>


// Global Print Any function (can be accessed directly from Insense as proc)

void printString_proc(void *this, void *handler, StringPNTR param ){
  // Write output
  if(param != NULL){
    StringPNTR str = NULL;
    DAL_assign(&str, param);
    printf("%s", str->data);
    DAL_decRef(str);
  }
}
