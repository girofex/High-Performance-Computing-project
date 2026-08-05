#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MATRIX_SIZE 10000

int main(int argc, char **argv)
{
    int rank, size;
    int i, j, k;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (MATRIX_SIZE % size != 0){
        if (rank == 0)
            fprintf(stderr, "MATRIX_SIZE (%d) must be divisible by number of processes (%d)\n",
                    MATRIX_SIZE, size);

        MPI_Finalize();
        return 1;
    }

    int rows_per_proc = MATRIX_SIZE / size;

    start = MPI_Wtime();

    //b (broadcast) and c (gathered) are needed on every rank
    double (*a)[MATRIX_SIZE] = NULL;
    double (*b)[MATRIX_SIZE] = malloc(sizeof(double[MATRIX_SIZE][MATRIX_SIZE]));
    double (*c_local)[MATRIX_SIZE] = malloc(sizeof(double[rows_per_proc][MATRIX_SIZE]));
    double (*a_local)[MATRIX_SIZE] = malloc(sizeof(double[rows_per_proc][MATRIX_SIZE]));

    if (rank == 0){
        a = malloc(sizeof(double[MATRIX_SIZE][MATRIX_SIZE]));   //allocated only on rank=0

        for (i = 0; i < MATRIX_SIZE; i++)
            for (j = 0; j < MATRIX_SIZE; j++)
                a[i][j] = 2.0;
    }

    for (i = 0; i < MATRIX_SIZE; i++)
        for (j = 0; j < MATRIX_SIZE; j++)
            b[i][j] = 3.0;

    for (i = 0; i < rows_per_proc; i++)
        for (j = 0; j < MATRIX_SIZE; j++)
            c_local[i][j] = 0.0;

    //scatter rows of a: each process gets rows_per_proc * MATRIX_SIZE doubles
    MPI_Scatter((rank == 0 ? a : NULL), rows_per_proc*MATRIX_SIZE, MPI_DOUBLE, a_local, rows_per_proc*MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //every process needs b
    MPI_Bcast(b, MATRIX_SIZE*MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (i = 0; i < rows_per_proc; ++i)
        for (k = 0; k < MATRIX_SIZE; k++)
            for (j = 0; j < MATRIX_SIZE; ++j)
                c_local[i][j] += a_local[i][k] * b[k][j];

    double (*c)[MATRIX_SIZE] = NULL;

    if (rank == 0)
        c = malloc(sizeof(double[MATRIX_SIZE][MATRIX_SIZE]));

    MPI_Gather(c_local, rows_per_proc*MATRIX_SIZE, MPI_DOUBLE, (rank == 0 ? c : NULL), rows_per_proc*MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0){
        FILE *f = fopen("mat-res.txt", "w");
        if (!f){
            perror("fopen");
            MPI_Finalize();
            return 1;
        }

        fprintf(f, "%d\n\n", MATRIX_SIZE);

        for (int ii = 0; ii < 1000; ii++){
            for (int jj = 0; jj < 1000; jj++)
                fprintf(f, "%.0f ", c[ii][jj]);

            fprintf(f, "\n");
        }

        fclose(f);
        free(a);
        free(c);
    }

    free(b);
    free(c_local);
    free(a_local);

    end = MPI_Wtime();
    if (rank == 0)
        printf("Time taken to multiply the matrices: %.2lf seconds.\n", end - start);

    MPI_Finalize();
    return 0;
}