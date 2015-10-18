#include "Component.h"

/**
 * Construct a new component object
 * @param[in] sourceFile String containing path to bytecode source file
 * @return Pointer to new component object
 */
Component component_constructor(char* sourceFile) {
    Component this = GC_MALLOC(sizeof(struct Component_s));
    size_t componentNameSize = strrchr(basename(sourceFile), '.')-basename(sourceFile);
    this->name = GC_MALLOC_ATOMIC(componentNameSize);
    strncpy(this->name, basename(sourceFile), componentNameSize);
    return this;
}

/**
 * Start a component running
 *
 */
void* component_run(void* component) {
    Component this = (Component)component;
    printf("Component %s", this->name);
}