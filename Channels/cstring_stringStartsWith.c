/*
 * cstring.c 
 *
 * to provide some common ops on strings in c
 *
 */

#include "cstring.h"
#include <string.h>

bool stringStartsWith(char *this, char *compare){
  unsigned i;
  for(i=0; i<strlen(compare); i++)
    if(this[i]!=compare[i])
      return false;
  return true;
}
