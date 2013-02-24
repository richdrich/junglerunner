#!/bin/sh

XTAIL=/Users/richard/Documents/eclipsemac_cpp/junglerunner/build/junglerunner

echo 'Hello from inputbox shell script'

in1=`$XTAIL --inputbox 'Enter value 1:'  60 10 2>&1` 

echo 'First input done, in1=' $in1

in2=`$XTAIL --inputbox 'Enter value 2:'  60 10 plunk 2>&1`

$XTAIL --msgbox $in1' and '$in2' entered'  60 10





