#define n 15000

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char **argv)
{
    int i, j, k;
    double start, end;

    start = omp_get_wtime();

    double (*a)[n] = malloc(sizeof(double[n][n]));
    double (*b)[n] = malloc(sizeof(double[n][n]));
    double (*c)[n] = malloc(sizeof(double[n][n]));

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
        {
            a[i][j] = 2.0;
            b[i][j] = 3.0;
            c[i][j] = 0.0;
        }

    #pragma omp parallel for default(none) shared(a,b,c) private(i,k,j)
    for (i = 0; i < n; ++i)
        for (k = 0; k < n; k++)
            for (j = 0; j < n; ++j)
                c[i][j] += a[i][k] * b[k][j];

    FILE *f = fopen("mat-res.txt", "w");
    if (!f)
    {
        perror("fopen");
        return 1;
    }

    fprintf(f, "%d\n\n", n);
    for (int i = 0; i < 1000; i++)
    {
        for (int j = 0; j < 1000; j++)
        {
            fprintf(f, "%.0f ", c[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);

    free(a);
    free(b);
    free(c);

    end = omp_get_wtime();

    printf("Time taken to multiply the matrices: %.2lf seconds.\n", end-start);

    return 0;
}