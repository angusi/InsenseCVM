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
#include "getstring.h"
#include <stdio.h>


// Global function to read a string from the serial line over USB (can be accessed directly from Insense as proc)

StringPNTR getString_proc(void *this, void *handler){
  StringPNTR result = Construct_String1(getstring());
  return result;
}
