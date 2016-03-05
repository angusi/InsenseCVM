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
#include "math.h"


float absReal_proc(void *this, void *handler, float r) {
  //return (r<0.0?-r:r);
  return fabs(r);
} 
