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
#include "GlobalVars.h"

//static bool success;


bool byteArrayEqual_proc(void *this, void *handler, IArrayPNTR a, IArrayPNTR b){
  if( a->length != b->length ) { 	
    return false;
  }
  int i;
  for(i=0; i<a->length; i++) { 
    if((*(uint8_t*)array_loc(a, i, &success)) != (*(uint8_t*)array_loc(b, i, &success ))) { 
      return false;
    }
  }
  return true;
}
  

