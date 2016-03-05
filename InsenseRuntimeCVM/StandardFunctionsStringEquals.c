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


bool stringEquals_proc(void *this, void *handler, StringPNTR param1, StringPNTR param2) {
	return (stringCompare(param1, param2) == 0);
} 
