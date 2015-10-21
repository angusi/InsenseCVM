#include "Component.h"
#include "BytecodeTable.h"

/**
 * Construct a new component object
 * @param[in] sourceFile String containing path to bytecode source file
 * @param[in] params Array of parameters
 * @param[in] paramCount Number of parameters
 * @return Pointer to new component object
 */
Component component_constructor(char* sourceFile, char* params[], int paramCount) {
    Component this = GC_MALLOC(sizeof(struct Component_s));
    size_t componentNameSize = strrchr(basename(sourceFile), '.')-basename(sourceFile);
    this->name = GC_MALLOC_ATOMIC(componentNameSize);
    strncpy(this->name, basename(sourceFile), componentNameSize);

    //TODO: Check file exists
    this->sourceFile = fopen(sourceFile, "r");

    return this;
}

/**
 * Start a component running
 * @param[in,out] component Component instance to run.
 */
void* component_run(void* component) {
    Component this = (Component)component;
    log_logMessage(INFO, this->name, "Start");

    int nextByte;
    while((nextByte = fgetc(this->sourceFile)) != EOF) {
        switch(nextByte) {
            case BYTECODE_ENTERSCOPE:
                log_logMessage(DEBUG, this->name, "ENTERSCOPE");
                break;
            case BYTECODE_CALL:
                log_logMessage(DEBUG, this->name, "CALL");
                break;
            default:
                log_logMessage(WARNING, this->name, "Unknown Byte Read");
                break;
        }
    }

    log_logMessage(INFO, this->name, "End");


    return NULL;
}

char* component_getName(Component this) {
    char* name = GC_MALLOC_ATOMIC(strlen(this->name)+1);
    strncpy(name, this->name, strlen(this->name));
    name[strlen(this->name)] = '\0';
    return name;
}