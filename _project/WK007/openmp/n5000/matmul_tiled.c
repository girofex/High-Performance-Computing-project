#define n 5000
#define BLOCK_SIZE 64

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

static inline int min_int(int a, int b) { return a < b ? a : b; }

int main(int argc, char **argv)
{
    int i, j, k, ii, jj, kk;
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

    #pragma omp parallel for default(none) shared(a, b, c) private(i, k, j, ii, kk, jj) schedule(dynamic)
    for (ii = 0; ii < n; ii += BLOCK_SIZE){
        int i_max = min_int(ii + BLOCK_SIZE, n);
        
        for (kk = 0; kk < n; kk += BLOCK_SIZE){
            int k_max = min_int(kk + BLOCK_SIZE, n);

            for (jj = 0; jj < n; jj += BLOCK_SIZE){
                int j_max = min_int(jj + BLOCK_SIZE, n);

                for (i = ii; i < i_max; ++i)
                    for (k = kk; k < k_max; ++k){
                        double a_ik = a[i][k];
                        for (j = jj; j < j_max; ++j)
                            c[i][j] += a_ik * b[k][j];
                    }
            }
        }
    }

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
            fprintf(f, "%.0f ", c[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);

    free(a);
    free(b);
    free(c);

    end = omp_get_wtime();
    printf("Time taken to multiply the matrices: %.2lf seconds.\n", end - start);

    return 0;
}