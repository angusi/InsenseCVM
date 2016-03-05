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


int squareInt_proc(void *this, void *handler, int i) { 
  return (i*i);
} 
