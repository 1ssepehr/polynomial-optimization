#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "complex.h"
#include "genetic.h"

int bitNumber; /* Total number of bits used for each coefficient */
int bitResolution;

/*** MAIN ***/
int main(int argc, char *argv[])
{
    int i, j;
    int p1, p2, c1, c2;
    int u_count = 0;
    chromosome sample[M_G];
    double *u, tmp;
    FILE *fin;
    FILE *fout_evaluation;
    // FILE *fout_generation;

    fout_evaluation = fopen("genetic_output_evaluation", "a");
    // fout_generation = fopen("genetic_output_generation", "a");

    /* Randomize with time */
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    srand(spec.tv_nsec);

    /* Read the total number of bits from argument 1 */
    bitResolution = atoi(argv[1]);
    bitNumber = bitResolution - 3;

    /* Count the u_i values from the file (filename via argv[1]) */
    fin = fopen("roots", "r");
    while (fscanf(fin, "%lf", &tmp) != EOF)
        u_count++;
    fclose(fin);

    /* Read the u_i values from the file (filename via argv[1]) */
    u = (double *)malloc(sizeof(double) * u_count);
    fin = fopen("roots", "r");
    for (i = 0; i < u_count; i++)
        fscanf(fin, "%lf", &u[i]);
    fclose(fin);

    /* Initialize the M_G chromosomes with random values */
    for (i = 0; i < M_G; i++)
    {
        for (j = 0; j <= N_DEGREE; j++)
            sample[i].theta[j] = new_theta(); /* Fill the thetas */
        evaluate(&sample[i], u, u_count);     /* Evaluate after filling */
    }
    for (j = 0; j <= N_DEGREE; j++)
        sample[M_G - 1].theta[j] = 0; /* The "all zero" chromosome */
    evaluate(&sample[M_G - 1], u, u_count);

    /* Until convergence, for N_REPEAT times: */
    for (i = 0; i < N_REPEAT; i++)
    {
        /* Select parent chromosomes from top half to mix */
        for (j = M_G / 2; j < M_G; j += 2)
        {
            p1 = rand() % (M_G / 2);
            do
                p2 = rand() % (M_G / 2);
            while (p1 == p2); /* p1, p2 -> [0, N/2) also p1 != p2 */

            c1 = j;
            c2 = j + 1;

            crossover(&sample[p1], &sample[p2], &sample[c1], &sample[c2]);

            mutate(&sample[c1]);
            mutate(&sample[c2]);

            evaluate(&sample[c1], u, u_count);
            evaluate(&sample[c2], u, u_count);
        }

        /* Rank the M_G chromosomes (using quicksort) */
        qsort((void *)sample, M_G, sizeof(chromosome), comparator);
    }

    /* Ouput the best chromosome theta values */
    fprintf(fout_evaluation, "%lf\n", sample[0].evaluation);
    fclose(fout_evaluation);
    return 0;
}

double calculate(chromosome *A, double u, int bitResolution)
{
    int k;
    complex ans, term;
    ans.real = ans.im = 0.0;
    for (k = 0; k <= N_DEGREE; k++)
    {
        term.real = cos((A->theta[k] - ((1 << bitNumber) - 1) / 2) * (2 * M_PI / (1 << bitResolution)) + k * M_PI * u);
        term.im   = sin((A->theta[k] - ((1 << bitNumber) - 1) / 2) * (2 * M_PI / (1 << bitResolution)) + k * M_PI * u);
        ans = Add_Complex(ans, term);
    }
    return 20 * log10(Norm_Complex(ans));
}

void evaluate(chromosome *A, double u[], int u_count)
{
    double max = -10e7;
    double p;
    int i;
    for (i = 0; i < u_count; i++)
    {
        p = calculate(A, u[i], bitResolution);
        if (max < p)
            max = p;
    }
    A->evaluation = max;
}

int comparator(const void *A, const void *B)
{
    double a, b;
    a = ((chromosome *)A)->evaluation;
    b = ((chromosome *)B)->evaluation;
    /* if A has higher results, it's worse, so it comes later => 1 (and -1 o.w.) */
    return (a > b) ? 1 : -1;
}

void mutate(chromosome *A)
{
    int i, j;
    for (i = 0; i <= N_DEGREE; i++)
        for (j = 0; j < bitNumber; j++)
            if (rand() % 100 < MUTATION)
                A->theta[i] ^= ('\0' + 1) << j;
}

void crossover(chromosome *p1, chromosome *p2, chromosome *c1, chromosome *c2)
{
    int k;
    unsigned char temp;
    *c1 = *p1;
    *c2 = *p2;
    for (k = 0; k <= N_DEGREE; k++)
    {
        if (rand() % 2 == 1)
        {
            temp = c1->theta[k];
            c1->theta[k] = c2->theta[k];
            c2->theta[k] = temp;
        }
    }
}

void print_chromosome(chromosome A)
{
    int i;
    printf("[");
    for (i = 0; i < N_DEGREE; i++)
        printf("%d, ", A.theta[i]);
    printf("%d]\n", A.theta[N_DEGREE]);
}