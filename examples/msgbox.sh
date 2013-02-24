#!/bin/sh
XTAIL=/Users/richard/Documents/eclipsemac_cpp/junglerunner/build/junglerunner

echo 'Hello from msgbox shell script'

$XTAIL --msgbox Continue? 60 10

echo 'Shell script done, user clicked ' $?



