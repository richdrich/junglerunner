/*
 * debug.c
 *
 *  Created on: Mar 3, 2013
 *      Author: richardparratt
 */

#include "debug.h"

bool debug_enabled = false;

void debugf(const char *fmt, ...) {
	if(!debug_enabled) {
		return;
	}

	va_list arglist;
	va_start( arglist, fmt );
	vfprintf(stderr,  fmt, arglist );
	va_end( arglist );
}



