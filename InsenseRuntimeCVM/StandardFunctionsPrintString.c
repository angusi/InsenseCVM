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


// Global Print Any function (can be accessed directly from Insense as proc)

void* StandardFunction_printString(int argc, void* argv[]){
    // Write output
    if(argc != 1) {
        log_logMessage(ERROR, "printInt_proc", "Unexpected argument count - expected 1, got %d", argc);
        return 0;
    }

    //TODO: Some kind of unescaping so we can have e.g. \n in the string?
    printf("%s\n", (char*)argv[0]);
    return 0;
}
