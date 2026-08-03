#define n 15000

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char **argv)
{
    int i, j, k;
    time_t start, end;

    start = time(NULL);

    MPI_Init(&argc, &argv);

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

    MPI_Scatter(a, n * n / MPI_COMM_WORLD, MPI_DOUBLE, a, n * n / MPI_COMM_WORLD, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (i = 0; i < n; ++i)
        for (k = 0; k < n; k++)
            for (j = 0; j < n; ++j)
                c[i][j] += a[i][k] * b[k][j];

    MPI_Gather(c, n * n / MPI_COMM_WORLD, MPI_DOUBLE, c, n * n / MPI_COMM_WORLD, MPI_DOUBLE, 0, MPI_COMM_WORLD);

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

    MPI_Finalize();

    end = time(NULL);

    printf("Time taken to multiply the matrices: %.2lf seconds.\n", difftime(end, start));

    return 0;
}