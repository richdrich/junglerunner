#!/bin/bash

	cat <<ENDS >ctemps/alltemplates.h
#ifndef ALLTEMPLATES_H
#define ALLTEMPLATES_H
static const char * AllTemplates[][2] = {
ENDS

count=0
first=true
while (( "$#" )); do
	if $first ; then
		first=false
	else
		echo ","  >>ctemps/alltemplates.h
	fi
	 
	echo '{ "'$1'", '  >>ctemps/alltemplates.h
	sed -f cstringize.sed templates/$1.htt >>ctemps/alltemplates.h
	
	echo "}" >>ctemps/alltemplates.h

	let count++
	shift
	done
	
echo "};" >>ctemps/alltemplates.h

echo "const int NumTemplates="$count";" >>ctemps/alltemplates.h

echo "#endif" >>ctemps/alltemplates.h

