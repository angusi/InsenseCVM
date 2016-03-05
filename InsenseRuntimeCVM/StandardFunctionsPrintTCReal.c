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
#include "math.h"
#include "print_tc.h"

#define MAX_REAL_STRING_LENGTH 16

// Global printTCReal function (can be accessed directly from Insense as proc)

void printTCReal_proc(void *this, void *handler, float f ){
	char real_string[MAX_REAL_STRING_LENGTH];
	int i = (int) f;
	if( i==0 && f < 0){
		sprintf(real_string, "-");
	}
	sprintf(real_string, "%i.%02i", i, (int) (fabs(f - i) * 100) );
	print_tc(real_string);
}
