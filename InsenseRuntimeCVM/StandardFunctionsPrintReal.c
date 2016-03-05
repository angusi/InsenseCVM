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


// Global Print Any function (can be accessed directly from Insense as proc)

void printReal_proc(void *this, void *handler, float f ){
	int i = (int) f;
	if( i==0 && f < 0){
		printf("-");
	}
	printf("%i.%02i", i, (int) (fabs(f - i) * 100) );
}
