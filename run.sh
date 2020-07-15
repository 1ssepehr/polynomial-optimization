#!/bin/bash

# variables
OUTPUT=./output.csv
REPETITION=100

# make and error check
make
if [ $? -eq 0 ]
then
    printf "Makefile successful.\n"
else
    exit 1
fi

# check for file existing
if [ -a $OUTPUT ]
then
    printf "$OUTPUT exists. Overwrite? y/n\n"
    read ANSWER
    case $ANSWER in
        [yY]) rm -f $OUTPUT
        ;;
        [nN]) printf "Exiting script.\n"; exit 1
        ;;
    esac
fi

# the header for .csv file
printf "bitRes, root, min, max, median, mean, stddev\n" >> $OUTPUT

# execute code
for targetBit in 2 3 4 5 6 7 8 9 10
do
    printf "\n" >> $OUTPUT
    for roots in 0.3 0.35 0.4 0.45 0.55
    do
        printf "%d, %.2f, " $targetBit $roots >> $OUTPUT        
        for ((i=0; i<$REPETITION; i++))
        do
            printf "$roots" | ./genetic 10 ${targetBit} 
        done | st --min --max --median --mean --stddev --format="%.2f" --no-header --delimiter=", " >> $OUTPUT            
    done
done

make clean