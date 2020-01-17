#!/bin/bash
gcc main.c complex.c -lm -o output
touch tmp
for i in {1..$1}
do
    ./output roots >> tmp
done
./mean < tmp
rm tmp
rm output
