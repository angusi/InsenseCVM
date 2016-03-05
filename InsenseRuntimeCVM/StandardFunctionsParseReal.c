/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include "StandardFunctions.h"
#include "DAL_mem.h"
#include "events.h"
#include "setjmp.h"
#include <stdio.h>

#define MAXINTLENGTH 10

// A function to parse a real from an Insense string

float parseReal_proc(void *this, void *handler, StringPNTR s){
	StringPNTR s_local = NULL;
	DAL_assign(&s_local, s);
	float f = (float) atoi(s_local->data);	
	int c = 0;
	for(;;){
		if(s_local->data[c] == '.'){
			c++;
			float f2 = (float) atoi((s_local->data+c));
			while(f2 > 1){
				f2 = f2 / 10;
			}
			if(f >= 0) f = f + f2; else f = f - f2;
			break;
		}
		if(s_local->data[c] == '\0'){
			break;
		}
		c++;
	} 
	DAL_assign(&s_local, NULL);
	return f;
}
