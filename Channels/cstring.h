/*
 * cstring.h 
 *
 * provide some common ops on strings in c
 *
 */

#ifndef C_STRING_H
#define C_STRING_H

#include <stdbool.h>
#include <string.h>

#define MAX_STRLENGTH 100

extern bool stringStartsWith(char *this, char *compare);
extern char *stringcat(char *s1, char *s2);
extern void memncpy(void *dest, void *src, unsigned size);

#endif /* C_STRING_H */
