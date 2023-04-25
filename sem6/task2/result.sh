#!/bin/bash

string=$(cat logs/log.out | grep 'Total time')
echo $string
time1=$(echo $string | cut -d ' ' -f3)
time2=$(echo $string | cut -d ' ' -f6)
time3=$(echo $string | cut -d ' ' -f9)
time4=$(echo $string | cut -d ' ' -f12)
time5=$(echo $string | cut -d ' ' -f15)
time6=$(echo $string | cut -d ' ' -f18)

if [ 1 -eq "$(echo "${time1} < ${time2}" | bc)" ]
then
    min=${time1}
else
    min=${time2}
fi
if [ 1 -eq "$(echo "${min} < ${time3}" | bc)" ]
then
    min=${min}
else
    min=${time3}
fi

if [ 1 -eq "$(echo "${min} < ${time4}" | bc)" ]
then
    min=${min}
else
    min=${time4}
fi

if [ 1 -eq "$(echo "${min} < ${time5}" | bc)" ]
then
    min=${min}
else
    min=${time5}
fi

if [ 1 -eq "$(echo "${min} < ${time6}" | bc)" ]
then
    min=${min}
else
    min=${time6}
fi


echo $min
