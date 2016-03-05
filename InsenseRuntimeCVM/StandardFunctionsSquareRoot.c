/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include "StandardFunctions.h"
#include "events.h"


// Not using exact functions for now, as including math library uses 3KB
// Instead using Babylonian method
// start with some x and do
// x_n+1 = 0.5*(x_n + r/x_n)

float squareRoot_proc(void *this, void *handler, float r) { 
  int i=0, n=20;
  float x=1.0;
  if(r>0){
    for(i=0; i<n; i++){
      x = 0.5*(x+r/x);
    }
    if(x < 0)
      return -x;
    else
      return x;
  }
  return 0.0;
} 
