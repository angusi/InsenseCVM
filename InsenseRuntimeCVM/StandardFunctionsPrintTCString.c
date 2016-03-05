/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include "StandardFunctions.h"
#include "events.h"
#include "DAL_mem.h"
#include <stdio.h>
#include "print_tc.h"

// Global Print Any function (can be accessed directly from Insense as proc)

void printTCString_proc(void *this, void *handler, StringPNTR param ){
  // Write output
  if(param != NULL){
    StringPNTR str = NULL;
    DAL_assign(&str, param);
    print_tc(str->data);
    DAL_decRef(str);
  }
}
