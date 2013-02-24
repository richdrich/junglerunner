#!/bin/sh
XTAIL=/Users/richard/Documents/eclipsemac_cpp/junglerunner/build/junglerunner

echo 'Hello from sequence shell script'

$XTAIL --msgbox 'Sequence script: Continue?' 60 10
res=$?

echo '#1: User clicked: '$res
if [ "$res" -eq "0" ]
then
  echo 'msgbox #2'
  $XTAIL --msgbox "User clicked Ok, keep going?" 60 10
  res=$?
  echo 'User clicked: '$res
  if [ "$res" -eq "0" ] 
  then
   echo 'Infobox Ok twice'
   $XTAIL --infobox "User clicked Ok twice, done" 60 10
  else
   $XTAIL --infobox "User clicked Ok then cancel, done" 60 10
  fi

else
 $XTAIL --infobox "User clicked Cancel, done" 60 10
fi

