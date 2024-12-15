#!/bin/bash

function get_minimum {
min=$1
for i in $@; do
    if [ 1 -eq "$(echo "$min > $i" | bc)" ]
    then
        min=$i
    fi
done
echo $min
}


for N in 128 256 512; do

echo "OMP"

for i in 1 2 4 8 16 32; do
    times=$(cat logs/log.out.$N.0.$i | grep "Time:" | tr -s '  ' ' ' | cut -d ' ' -f3)
    echo $N $i $(get_minimum $times)
done

echo "MPI"

for i in 1 2 4 8 16 32; do
    times=$(cat logs/log.out.$N.$i.1 | grep "Time:" | tr -s '  ' ' ' | cut -d ' ' -f3)
    echo $N $i $(get_minimum $times)
done

echo "MPI+OMP"

for i in 1 2 4 8; do
    times=$(cat logs/log.out.$N.$i.4 | grep "Time:" | tr -s '  ' ' ' | cut -d ' ' -f3)
    echo $N $i $(get_minimum $times)
done

done
