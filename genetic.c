#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "complex.h"
#include "genetic.h"

int bitCount;
int bitResolution;
int targetBitRes;

/* The program receives the global variables above via command args. */
/* The roots for the nulling points of the polynomial come from stdin. */
/*** MAIN ***/
int main(int argc, char *argv[])
{
    int i, j;
    int p1, p2, c1, c2;
    chromosome sample[M_G];
    double *roots, nextU;
    int rootsCount = 0;

    /* Randomize with time */
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    srand(spec.tv_nsec);

    /* Read variables from command arguments */
    bitResolution = atoi(argv[1]);
    targetBitRes = atoi(argv[2]);
    bitCount = bitResolution - 1;

    /* Input the roots from standard input */
    roots = NULL;
    while (scanf("%lf", &nextU) != EOF)
    {
        rootsCount++;
        roots = (double *)realloc(roots, sizeof(double) * rootsCount);
        roots[rootsCount - 1] = nextU;
    }

    /* Initialize the M_G chromosomes with random values */
    for (i = 0; i < M_G; i++)
    {
        for (j = 0; j <= N_DEGREE; j++)
            sample[i].theta[j] = new_theta();       /* Fill the thetas */
        evaluate(&sample[i], roots, rootsCount, 1); /* Evaluate after filling */
    }
    for (j = 0; j <= N_DEGREE; j++)
        sample[M_G - 1].theta[j] = 0; /* The "all zero" chromosome */
    evaluate(&sample[M_G - 1], roots, rootsCount, 1);

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

            evaluate(&sample[c1], roots, rootsCount, 1);
            evaluate(&sample[c2], roots, rootsCount, 1);
        }

        /* Rank the M_G chromosomes (using quicksort) */
        qsort((void *)sample, M_G, sizeof(chromosome), comparator);
    }

    /* Ouput the best chromosome theta values */

    evaluate(&sample[0], roots, rootsCount, 0);

    printf("%lf\n", sample[0].evaluation);
    return 0;
}

double calculate(chromosome *A, double u, int bitResolution, int use_exact_angle)
{
    int k;
    complex ans, term, mainlobe;
    ans.real = ans.im = 0.0;
    mainlobe.real = mainlobe.im = 0.0;
    double angle;
    for (k = 0; k <= N_DEGREE; k++)
    {
        angle = get_exact_angle(A, 0, k);
        term.real = cos(angle);
        term.im = sin(angle);
        mainlobe = Add_Complex(mainlobe, term);
    }
    for (k = 0; k <= N_DEGREE; k++)
    {
        angle = use_exact_angle ? get_exact_angle(A, u, k) : get_angle(A, u, k);
        term.real = cos(angle);
        term.im = sin(angle);
        ans = Add_Complex(ans, term);
    }
    return -20 * log10(Norm_Complex(Divide_Complex(mainlobe, ans)));
}

void evaluate(chromosome *A, double u[], int u_count, int use_exact_angle)
{
    double max = -10e7;
    double p;
    int i;
    for (i = 0; i < u_count; i++)
    {
        p = calculate(A, u[i], bitResolution, use_exact_angle);
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
        for (j = 0; j < bitCount; j++)
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

double get_exact_angle(chromosome *A, double u, int k)
{
    return (A->theta[k] - ((1 << bitCount) - 1.0) / 2.0) * (2 * M_PI / (1 << bitResolution)) + k * M_PI * u;
}

double get_angle(chromosome *A, double u, int k)
{
    double stepAngle = 2 * M_PI / (1 << targetBitRes);
    double angle;
    double roundedAngle;
    angle = get_exact_angle(A, u, k);
    if (targetBitRes == bitResolution)
        return angle;
    while (angle > 0)
        angle = angle - 2 * M_PI;
    while (angle < 0)
        angle = angle + 2 * M_PI;

    int steps = (int)(floor(angle / stepAngle));
    roundedAngle = steps * stepAngle;

    while (roundedAngle < angle)
    {
        roundedAngle += stepAngle;
    }
    if ((roundedAngle - angle) > 0.5 * stepAngle)
        roundedAngle -= stepAngle;

    // printf("%f %f %f %d %d \n", angle, roundedAngle, stepAngle, bitResolution, targetBitRes);

    return roundedAngle;
}

void print_chromosome(chromosome A)
{
    int i;
    printf("[");
    for (i = 0; i < N_DEGREE; i++)
        printf("%d, ", A.theta[i]);
    printf("%d]\n", A.theta[N_DEGREE]);
}