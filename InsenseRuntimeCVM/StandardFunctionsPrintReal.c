/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include <math.h>
#include "StandardFunctions.h"
#include "../Logger/Logger.h"


// Global Print Any function (can be accessed directly from Insense as proc)

void* StandardFunction_printReal(int argc, void* argv[]){
	if(argc != 1) {
		log_logMessage(ERROR, "printReal_proc", "Unexpected argument count - expected 1, got %d", argc);
		return 0;
	}
	int i = (int) argv[0];
	if( i==0 && argv[0] < 0){
		printf("-");
	}
	printf("%i.%02i", i, (int) (fabs(*(double*)argv[0] - i) * 100) );
}
