#!/bin/bash

# run.sh [$1 = how-many-times] [$2 = output]

gcc main.c complex.c -lm -o output
for (( i = 0; i < $1; i++ )) 
do
    ./output roots >> $2
done
