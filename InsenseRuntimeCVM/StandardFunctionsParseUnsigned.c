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


// A function to parse an integer from an Insense string

unsigned parseUnsignedInt_proc(void *this, void *handler, StringPNTR s){
	return (unsigned)parseInt_proc(this, handler, s);
}
