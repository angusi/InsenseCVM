#ifndef CVM_STRINGS_H
#define CVM_STRINGS_H

#if defined(WIN32) || defined(_WIN32)
static const char PATH_SEPARATOR = '\\';
#else
static const char PATH_SEPARATOR = '/';
#endif

static const char* PROGRAM_NAME = "Insense C Virtual Machine";
static const char* PROGRAM_VERSION = "0.0.1";
static const char* PROGRAM_USAGE = "Usage: %s <program directory>\n";

#endif //CVM_STRINGS_H
