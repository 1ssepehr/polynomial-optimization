#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "complex.h"
#include "bitwise.h"

int main( int argc, char* argv[] ) {

    int i;
    int u_count = 0;
    double u[1];

    /* Count the u_i values from the file (filename via argv[1]) */
    /* fin = fopen( "roots", "r" ); */
    /* while( fscanf( fin, "%lf", &tmp ) != EOF ) u_count++; */
    /* fclose( fin ); */
    /* Read the u_i values */
    /* u = (double *) malloc( sizeof(double) * u_count ); */
    /* fin = fopen( "roots", "r" ); */
    /* for( i = 0; i < u_count; i++) */
        /* fscanf( fin, "%lf", &u[i] ); */

    /* Single root taken from argv[1] as a float */
    u[0] = atof( argv[1] );
    u_count = 1;

    chromosome template;
    for( i = 0; i <= N_DEGREE; i++ ) template.theta[i] = 0;

    template = optimize_bitwise( 0, template, u, u_count );
    template = optimize_bitwise( 1, template, u, u_count );
    template = optimize_bitwise( 2, template, u, u_count );
    printf( "%lf\n", template.evaluation );
    return 0;
}

chromosome optimize_bitwise( int bit_pos, chromosome template, double* u, int u_count ) {

    int i, j;
    int jth_bit_of_i, template_with_pos_zerod_out;

    chromosome dataset[65536];
    for( i = 0; i < 65536; i++ ) {
        for( j = 0; j <= N_DEGREE; j++ ) {

            jth_bit_of_i                = ( (i & (1 << j)) != 0 );
            template_with_pos_zerod_out = template.theta[j] & ~(1 << (TOTAL_BITS - 1 - bit_pos));

            dataset[i].theta[j] = template_with_pos_zerod_out | (jth_bit_of_i << (TOTAL_BITS - 1 - bit_pos));
        }
        evaluate( &dataset[i], u, u_count, bit_pos + 1 ); /* Evaluate after filling */
    }
    qsort( (void*) dataset, 65536, sizeof( chromosome ), comparator );
    return dataset[0];
}

double calculate( chromosome *A, double u, int flag ) {
    int i, k;
    complex ans, term;
    double region;
    int nth_bit_of_theta, power_of_two;
    ans.real = ans.im = 0.0;

    for( k = 0; k <= N_DEGREE; k++ ) {

        /**************************************************************/
        /* Here is how the region is found for the calculation.       */
        /* Bits are counted from the most significant to the least.   */
        /* Flag is the index of the first bit that doesn't count.     */
        /* That is, bits after flag are 0.5 to be on the mean         */
        /*                                                            */
        /* Example:                                                   */
        /* 101X ==> 3                                                 */
        /* _____________________________________                      */
        /* 1        0        1         X                              */
        /* 1 * 8    0 * 4    1 * 2     0.5 * 1                        */
        /* _____________________________________                      */
        /* 8 + 0 + 2 + 0.5 = 10.5  (sum)                              */
        /* 4 bits ==> offset = -7.5 (range is from -7.5 to 7.5)       */
        /* (total span of 15 = 2^4 - 1)                               */
        /* region = sum + offset = 3                                  */
        /**************************************************************/

        region = 0.5 - (1 << (TOTAL_BITS - 1)); /* The offset value */

        /* For bits before the flag, take real value (be it 0 or 1) */
        for( i = 0; i < flag; i++ ) {
            power_of_two = 1 << (TOTAL_BITS - 1 - i);
            nth_bit_of_theta = ((A->theta[k] & power_of_two) != 0);
            region += nth_bit_of_theta * power_of_two;
        }

        /* For bits after the flag, they are considered 0.5 */
        for( i = flag; i < TOTAL_BITS; i++) {
            power_of_two = 1 << (TOTAL_BITS - 1 - i);
            region += 0.5 * power_of_two;
        }

        term.real = cos( region * (2*M_PI / 64) + k * M_PI * u );
        term.im   = sin( region * (2*M_PI / 64) + k * M_PI * u );
        ans = Add_Complex( ans, term );
    }
    return 20 * log10( Norm_Complex( ans ) );
}

void evaluate( chromosome *A, double u[], int u_count, int flag ) {
    double min = 10e7;
    double p;
    int i;
    for( i = 0; i < u_count; i++ ) {
        p = calculate( A, u[i], flag );
        if( min > p ) min = p;
    }
    A -> evaluation = min;
}

int comparator( const void *A, const void *B ) {

    /* if A has higher results, it's worse, so it comes later => 1 (and -1 o.w.) */
    return ( ((chromosome*) A)->evaluation > ((chromosome*) B)->evaluation ) ? 1 : -1;
}
