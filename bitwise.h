#ifndef _BITWISE_H
#define _BITWISE_H

#define N_DEGREE ( 15 )   /* Degree of the polynomial */
#define TOTAL_BITS ( 3 )  /* Total number of bits used for each coefficient */

typedef struct {
    unsigned char theta[N_DEGREE + 1]; /* To cover theta_0 to theta_N, inclusive */
    double evaluation; /* The largest value of the polynomial when plugging all u_i */
} chromosome;


/* Calculates the polynomial in a given u  */
double calculate( chromosome *A, double u, int flag );

/* Evaluates a chromosome by plugging all different u values,
   to find its worst case: max{ P(u_i) for i in [1..u_count] } */
void evaluate( chromosome *A, double u[], int u_count, int flag );

/* comparator function for chromosome to use in quicksort */
int comparator( const void *A, const void *B );

/* Optimizes the bit_pos bit of all coefficients of a template chromosome with given u */
chromosome optimize_bitwise( int bit_pos, chromosome template, double* u, int u_count );

#endif
