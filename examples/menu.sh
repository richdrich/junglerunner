#!/bin/sh
XTAIL=/Users/richard/Documents/eclipsemac_cpp/junglerunner/build/junglerunner

echo 'Hello from menu shell script'

set=`$XTAIL --menu 'Select?' 60 10 4 A 'Option A' B 'Option B' 2>&1`
#$XTAIL --menu 'Select?' 60 10 4 A 'Option A' B 'Option B'

echo 'Shell script done, user clicked '$set