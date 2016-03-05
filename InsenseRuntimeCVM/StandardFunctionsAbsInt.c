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


int absInt_proc(void *this, void *handler, int i) {
  return (i<0?-i:i);
}
