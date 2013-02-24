#!/bin/bash

while (( "$#" )); do
	cat <<ENDS >ctemps/$1.h
#ifndef $1_H
#define $1_H
static const char * $1 = 
ENDS
	
	sed -f cstringize.sed templates/$1.htt >>ctemps/$1.h
	
	cat <<ENDS2 >>ctemps/$1.h
;
#endif
ENDS2

	shift
	done

