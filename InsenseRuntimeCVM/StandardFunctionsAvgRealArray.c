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


float avgRealArray_proc(void *this, void *handler, IArrayPNTR rarray) { 
  float avg = 0.0;
  int i;
  for( i=0; i< rarray->length; i++)
    avg += *(float*)array_loc( rarray, i, &success ) ;
  return avg/rarray->length;
}
