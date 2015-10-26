//
// Insense Virtual Machine
// Logger
//

#ifndef CVM_LOGGER_H
#define CVM_LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ExitCodes.h"

typedef enum { DEBUG, INFO, WARNING, ERROR, FATAL } log_Level_t;

void log_logMessage(log_Level_t level, const char* source, const char* message);
void log_setLogLevel(const char* level);
#endif //CVM_LOGGER_H