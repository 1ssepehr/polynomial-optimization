#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "complex.h"

#define new_theta() ( rand() % 8 )

#define N_DEGREE ( 15 )   /* Degree of the polynomial */
#define M_G      ( 700 )  /* Sample size population */
#define N_REPEAT ( 25 )   /* # of generations to go */

int TOTAL_BITS;          /* Total number of bits used for each coefficient */

#define MUTATION ( 5 )   /* % rate of mutating (toggling) bits of child chromosomes */

typedef struct {
    unsigned char theta[N_DEGREE + 1]; /* To cover theta_0 to theta_N, inclusive */
    double evaluation; /* The largest value of the polynomial when plugging all u_i */
} chromosome;

/* Calculates the polynomial in a given u  */
double calculate( chromosome *A, double u );

/* Evaluates a chromosome by plugging all different u values,
   to find its worst case: max{ P(u_i) for i in [1..u_count] } */
void evaluate( chromosome *A, double u[], int u_count );

/* comparator function for chromosome to use in quicksort */
int comparator( const void *A, const void *B );

/* Mutates a chromosome */
void mutate( chromosome *A );

/* Crossovers two parent chromosomes */
void crossover( chromosome *p1, chromosome *p2, chromosome *c1, chromosome *c2 );

/*** MAIN ***/
int main( int argc, char* argv[] ) {

    int i, j;
    int p1, p2, c1, c2;
    int u_count = 0;
    chromosome sample[M_G];
    double u[1];

    /* Randomize with time */
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    srand(spec.tv_nsec);

    /* Read the total number of bits from argument 1 */
    TOTAL_BITS = atoi( argv[1] );

    /* Count the u_i values from the file (filename via argv[1]) */
    fin = fopen( "roots", "r" );
    while( fscanf( fin, "%lf", &tmp ) != EOF ) u_count++;
    fclose( fin );

    /* Read the u_i values from the file (filename via argv[1]) */
    u = (double *) malloc( sizeof(double) * u_count );
    fin = fopen( argv[1], "r" );
    for( i = 0; i < u_count; i++)
        fscanf( fin, "%lf", &u[i] );

    /* Initialize the M_G chromosomes with random values */
    for( i = 0; i < M_G; i++ ) {
        for( j = 0; j <= N_DEGREE; j++ )
            sample[i].theta[j] = new_theta(); /* Fill the thetas */
        evaluate( &sample[i], u, u_count ); /* Evaluate after filling */
    }

    /* Until convergence, for N_REPEAT times: */
    for( i = 0; i < N_REPEAT; i++) {

        /* Select parent chromosomes from top half to mix */
        for( j = M_G / 2; j < M_G; j += 2 ) {

            p1 = rand() % (M_G/2);
            do p2 = rand() % (M_G/2); while (p1 == p2); /* p1, p2 -> [0, N/2) also p1 != p2 */

            c1 = j;
            c2 = j + 1;

            crossover( &sample[p1], &sample[p2], &sample[c1], &sample[c2] );

            mutate( &sample[c1] );
            mutate( &sample[c2] );

            evaluate( &sample[c1], u, u_count );
            evaluate( &sample[c2], u, u_count );
        }

        /* Rank the M_G chromosomes (using quicksort) */
        qsort( (void*) sample, M_G, sizeof( chromosome ), comparator );
    }

    /* Ouput the best chromosome theta values */
    printf( "%lf\n", sample[0].evaluation );
    return 0;
}

double calculate( chromosome *A, double u ) {
    int k;
    complex ans, term;
    ans.real = ans.im = 0.0;
    for( k = 0; k <= N_DEGREE; k++ ){
        term.real = cos( (A -> theta[k] - 3.5) * (2*M_PI / 64) + k * M_PI * u );
        term.im   = sin( (A -> theta[k] - 3.5) * (2*M_PI / 64) + k * M_PI * u );
        ans = Add_Complex( ans, term );
    }
    return 20 * log10( Norm_Complex( ans ) );
}

void evaluate( chromosome *A, double u[], int u_count ) {
    double min = 10e7;
    double p;
    int i;
    for( i = 0; i < u_count; i++ ) {
        p = calculate( A, u[i] );
        if( min > p ) min = p;
    }
    A -> evaluation = min;
}

int comparator( const void *A, const void *B ) {
    double a, b;
    a = ((chromosome*) A) -> evaluation;
    b = ((chromosome*) B) -> evaluation;
    /* if A has higher results, it's worse, so it comes later => 1 (and -1 o.w.) */
    return ( a > b ) ? 1 : -1;
}

void mutate( chromosome *A ) {
    int i, j;
    for( i = 0; i <= N_DEGREE; i++ )
        for( j = 0; j < TOTAL_BITS; j++ )
            if( rand() % 100 < MUTATION ) A -> theta[i] ^= ('\0' + 1) << j;
}

void crossover( chromosome *p1, chromosome *p2, chromosome *c1, chromosome *c2 ) {
    int k;
    unsigned char temp;
    *c1 = *p1;
    *c2 = *p2;
    for( k = 0; k <= N_DEGREE; k++ ) {
        if( rand() % 2 == 1 ) {
            temp           = c1 -> theta[k];
            c1 -> theta[k] = c2 -> theta[k];
            c2 -> theta[k] = temp;
        }
    }
}
