/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include <math.h>
#include <stdio.h>
#include "StandardFunctions.h"
#include "../Logger/Logger.h"


// Global Print Any function (can be accessed directly from Insense as proc)

void* StandardFunction_printReal(int argc, void* argv[]){
    if(argc != 1) {
        log_logMessage(ERROR, "printReal_proc", "Unexpected argument count - expected 1, got %d", argc);
        return 0;
    }
    double r = *(double*) argv[0];
    printf("%G", r);
    return 0;
}
