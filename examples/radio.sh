#!/bin/sh
XTAIL=/Users/richard/Documents/eclipsemac_cpp/junglerunner/build/junglerunner

echo 'Hello from menu shell script'

set=`$XTAIL --radiolist 'Select?' 60 10 4 100 'Option 100' 101 'Option 101' 200 'Option 200' 2>&1`
#$XTAIL --menu 'Select?' 60 10 4 100 'Option 100' 101 'Option 101' 200 'Option 200'

echo 'Shell script done, user clicked '$set