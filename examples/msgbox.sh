#!/bin/sh
XTAIL=/Users/richard/Documents/eclipsemac_cpp/junglerunner/build/junglerunner

echo 'Hello from msgbox shell script'

$XTAIL --msgbox Continue? 60 10 --ok-button 'Sure!'

echo 'Shell script done, user clicked ' $?



