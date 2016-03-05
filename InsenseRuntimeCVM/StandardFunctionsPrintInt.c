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

void printInt_proc(void *this, void *handler, int param ){
    
	printf("%d", param);
}
