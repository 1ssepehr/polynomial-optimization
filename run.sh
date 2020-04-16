#!/bin/bash

make
rm -f genetic_output_evaluation
rm -f genetic_output_generation
rm results_4_15_20

for delta in $(seq 0 0.00390625 0.0625)
do
    printf "Delta=%.0f/256:\n" $(echo "$delta * 256" | bc) >> results_4_15_20
    
    for roots in 0.125 0.25 0.375 0.5 0.625 0.75 0.875 -0.875 -0.75 -0.625 -0.5 -0.375 -0.25 -0.125
    do
        printf "___\n" >> results_4_15_20
        printf "Root=%.3f:\n" $roots >> results_4_15_20
        echo $(echo "$roots+$delta" | bc) > roots
        
        for i in {1..250}
        do
            ./genetic 3 40
        done
            
        printf "Evaluation " >> results_4_15_20
        st --min --max --median --mean --stddev --format="%.2f" --no-header --delimiter=", " genetic_output_evaluation >> results_4_15_20
        printf "Generation " >> results_4_15_20
        st --min --max --median --mean --stddev --format="%.2f" --no-header --delimiter=", " genetic_output_generation >> results_4_15_20

        rm -f genetic_output_evaluation
        rm -f genetic_output_generation
    done
done
printf "\n"