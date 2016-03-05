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
#include <stdio.h>
#include "print_tc.h"

#define MAX_UNSIGNED_STRING_LENGTH 8

// Global printTCUnsignedInt function (can be accessed directly from Insense as proc)

void printTCUnsignedInt_proc(void *this, void *handler, unsigned param ){
    char uint_string[MAX_UNSIGNED_STRING_LENGTH];
	sprintf(uint_string, "%u", param);
	print_tc(uint_string);
}
