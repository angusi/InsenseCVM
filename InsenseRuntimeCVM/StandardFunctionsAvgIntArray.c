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


float avgIntArray_proc(void *this, void *handler, IArrayPNTR iarray) { 
  float avg = 0.0;
  int i;
  for( i=0; i< iarray->length; i++)
    avg += *(int*)array_loc( iarray, i, &success ) ;
  return avg/iarray->length;
}
