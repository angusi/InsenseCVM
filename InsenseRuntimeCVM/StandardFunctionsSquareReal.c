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


float squareReal_proc(void *this, void *handler, float r) { 
  return (r*r);
} 
