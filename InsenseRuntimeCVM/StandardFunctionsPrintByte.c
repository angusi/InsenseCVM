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
#include "sched_arch.h"
#include <stdio.h>


// Global printInt function (can be accessed directly from Insense as proc)

void printByte_proc(void *this, void *handler, uint8_t param ){
	printf("%u", param);
}
