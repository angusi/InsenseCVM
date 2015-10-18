#ifndef CVM_MAIN_H
#define CVM_MAIN_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "Component.h"
#include "Strings.h"
#include "ExitCodes.h"
#include "include/gc/gc.h"

int main(int argc, char* argv[]);
char* getMainFilePath(char* directory);

#endif //CVM_MAIN_H
