/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include "StandardFunctions.h"
#include "../Logger/Logger.h"
#include <stdio.h>


// Global printInt function (can be accessed directly from Insense as proc)

void* StandardFunction_printInt(int argc, void* argv[]){
    if(argc != 1) {
        log_logMessage(ERROR, "printInt_proc", "Unexpected argument count - expected 1, got %d", argc);
        return 0;
    }
    printf("%d", *(int*)argv[0]);
    return 0;
}
