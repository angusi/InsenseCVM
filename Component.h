#ifndef CVM_COMPONENT_H
#define CVM_COMPONENT_H

#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include "include/gc/gc.h"

struct Component_s {
    char* name;
};

typedef struct Component_s* Component;

Component component_constructor(char* sourceFile);
void* component_run(void* this);

#endif //CVM_COMPONENT_H
