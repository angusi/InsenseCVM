#ifndef CVM_COMPONENT_H
#define CVM_COMPONENT_H

#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include "GC/GC_mem.h"
#include "Logger.h"

struct Component_s {
    char* name;
    FILE *sourceFile;
};

typedef struct Component_s* Component;

Component component_constructor(char* sourceFile, char* params[], int paramCount);
void* component_run(void* this);
char* component_getName(Component this);

#endif //CVM_COMPONENT_H
