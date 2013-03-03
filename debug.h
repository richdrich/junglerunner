/*
 * debug.h
 *
 *  Created on: Mar 3, 2013
 *      Author: richardparratt
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>
#include <stdarg.h>

extern bool debug_enabled;

void debugf(const char *fmt, ...);


#endif /* DEBUG_H_ */
