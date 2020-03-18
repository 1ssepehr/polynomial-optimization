#!/bin/bash

make
rm -f compare_genetic_bitwise
rm -f TEMP

for i in $(seq 0 0.01 1)
do
    rm -f TEMP
    for j in {1..100}
    do
        ./genetic $i >> TEMP
    done
    printf "U = %.2f\tGenetic: %f\tBitwise: %f\n" $i $(st --median TEMP) $(./bitwise $i) >> compare_genetic_bitwise
    printf "\nU = %.2f: done" $i
    rm -f TEMP
done
printf "\n"
