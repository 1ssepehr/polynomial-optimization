#ifndef GENETIC_H
#define GENETIC_H

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define new_theta() (rand() % (1 << bitCount))

#define N_DEGREE (15)  /* Degree of the polynomial */
#define M_G (64)       /* Sample size population */
#define N_REPEAT (100) /* # of generations to go */

#define MUTATION (5) /* % rate of mutating (toggling) bits of child chromosomes */

typedef struct
{
    unsigned char theta[N_DEGREE + 1]; /* To cover theta_0 to theta_N, inclusive */
    double evaluation;                 /* The largest value of the polynomial when plugging all u_i */
} chromosome;

/* Print the values of a chromosome in stdout */
void print_chromosome(chromosome A);

/* Calculates the polynomial in a given u  */
double calculate(chromosome *A, double u, int bitResolution, int use_exact_angle);

/* Evaluates a chromosome by plugging all different u values,
   to find its worst case: max{ P(u_i) for i in [1..u_count] } */
void evaluate(chromosome *A, double u[], int u_count, int use_exact_angle);

/* comparator function for chromosome to use in quicksort */
int comparator(const void *A, const void *B);

/* Mutates a chromosome */
void mutate(chromosome *A);

/* Crossovers two parent chromosomes */
void crossover(chromosome *p1, chromosome *p2, chromosome *c1, chromosome *c2);

double get_angle(chromosome *A, double u, int k);

double get_exact_angle(chromosome *A, double u, int k);

#endif