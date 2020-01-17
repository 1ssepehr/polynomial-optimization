#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "complex.h"

#define new_theta() ( rand() % 64 )

#define N_DEGREE ( 15 )  // Degree of the polynomial
#define N_ROOT ( 1 )     // Number of roots. 

#define M_G ( 700 )      // Sample size population
#define N_REPEAT ( 25 )  // # of generations to go

#define TOTAL_BITS ( 6 ) // Total number of bits used for each coefficient
#define MUTATION ( 5 )   // % rate of mutating (toggling) bits of child chromosomes

struct chromosome {
    unsigned char theta[N_DEGREE + 1]; // To cover theta_0 to theta_N, inclusive
    double evaluation; // The largest value of the polynomial when plugging all u_i
};

/**
 * Calculates the polynomial in a given u
 */
double calculate( struct chromosome *A, double z ) {
    int k; 
    complex ans, term;
    ans.real = 0; ans.im = 0;
    for( k = 0; k <= N_DEGREE; k++ ){
	term.real = cos( A->theta[i] * (2 * M_PI / 64) + k * M_PI * z );
	term.im   = sin( A->theta[i] * (2 * M_PI / 64) + k * M_PI * z );
	ans = Add_Complex( ans, term );
    }
    return Norm_Complex( ans );
}

/**
 * Evaluates a chromosome by plugging all different u values,
 * to find its worst case: max{ P(u_i) for i in [1..N_ROOT] }
 */
void evaluate( struct chromosome *A, double u[] ){
    double min = 10e7, p;
    int i;
    for( i = 0; i <= N_ROOT; i++ ) {
	p = calculate( A, u[i] );
	if( min > p ) min = p;
    }
    A -> evaluation = min;
}

/**
 * comparator function for chromosome to use in quicksort
 */
int comparator( const void *A, const void *B ) {
    double a, b;
    a = ((struct chromosome*) A) -> evaluation; 
    b = ((struct chromosome*) B) -> evaluation;
    /* if( fabs(a - b) < 10e-7 ) return 0; // float_absolute the difference to catch equal cases */  // --> unnecessary
    return ( a > b ) ? 1 : -1; // if A has higher results, it's worse, so it comes later => 1 (and -1 o.w.)
}

int main( int argc, char* argv[] ) {
    int i, j, k, l, p1, p2, c1, c2;
    struct chromosome sample[M_G], baby;
    double u[N_ROOT];
    
    // Read the u_i values from the file (filename given in argv[1])
    FILE* fin = fopen( argv[1], "r" );
    for( i = 0; i < N_ROOT; i++ )
	fscanf( fin, "%lf", &u[i] );

    // Randomize with time
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    srand(spec.tv_nsec); 

    // Initialize the M_G chromosomes with random values
    for( i = 0; i < M_G; i++ )
	for( j = 0; j <= N_DEGREE; j++ )
	    sample[i].theta[j] = new_theta(); // Fill the thetas

    // Evaluate all chromosomes
    for( i = 0; i < M_G; i++ )
	evaluate( &sample[i], u );

    /// Until convergence, for N_REPEAT times:
    for( i = 0; i < N_REPEAT; i++) {
	
	for( j = 0; j < M_G / 4; j++ ) {
	    // Select parent chromosomes from top half to mix
	    p1 = rand() % (M_G/2 + 1);
	    do p2 = rand() % (M_G/2 + 1); while (p1 == p2); // p1, p2 -> [0, N/2] also p1 != p2
	    c1 = M_G/2 + 2*j;
	    c2 = c1 + 1;

	    sample[c1] = sample[p1];
	    sample[c2] = sample[p2];
	    
	    // Mix to get new children
	    for( k = 0; k <= N_DEGREE; k++ ) {
		if( rand() % 2 ){ // swap [half] the thetas in the child chromosomes 
		    int temp = sample[c1].theta[k];
		    sample[c1].theta[k] = sample[c2].theta[k];
		    sample[c2].theta[k] = temp;
		}
	    }
	}
	
	// Mutate the children to maintain diversity
	for( j = M_G / 2; j < M_G; j++ ) // In bottom half of chromosomes
	    for( k = 0; k <= N_DEGREE; k++ ) // Traverse the coefficients
		for( l = 0; l < TOTAL_BITS; l++ ) // For each bit 
		    if( rand() % 100 < MUTATION ) 
			sample[j].theta[k] ^= ('\0' + 1) << l;
	
	// Evaluate all chromosomes again
	for( i = 0; i < M_G; i++ )
	    evaluate( &sample[i], u );

	// Rank the M_G chromosomes (using quicksort)
	qsort( (void*) sample, M_G, sizeof( struct chromosome ), comparator );

    }

    // Ouput the best chromosome theta values
    printf( "%lf\n", sample[0].evaluation );

    return 0;
}





















    
	

