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

#define MAX_INT_STRING_LENGTH 8

// Global printTCInt function (can be accessed directly from Insense as proc)

void printTCInt_proc(void *this, void *handler, int param ){
    char int_string[MAX_INT_STRING_LENGTH];
	sprintf(int_string, "%d", param);
	print_tc(int_string);
}
