#!/bin/bash

# run.sh [$1 = how-many-times] [$2 = output]

# script to bulk test results into a text file

make

for (( i = 0; i < $1; i++ ))
do
    ./output roots >> $2
done
