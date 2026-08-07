#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define n 10000
#define MIN(a,b) ((a) < (b) ? (a) : (b))

int main(int argc, char **argv)
{
    int rank, size;
    int i, j, k;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //load-balanced row distribution
    int rows_per_proc = n / size;
    int remainder = n % size;
    int local_rows = rows_per_proc + (rank < remainder ? 1 : 0);
    int row_start = rank * rows_per_proc + MIN(rank, remainder);

    //sendcounts/displs (in doubles) needed by rank 0 for Scatterv/Gatherv
    int *sendcounts = NULL;
    int *displs = NULL;

    if (rank == 0){
        sendcounts = malloc(sizeof(int) * size);
        displs = malloc(sizeof(int) * size);

        for (i = 0; i < size; i++){
            int proc_rows = rows_per_proc + (i < remainder ? 1 : 0);
            sendcounts[i] = proc_rows * n;
            displs[i] = (i * rows_per_proc + MIN(i, remainder)) * n;
        }
    }

    start = MPI_Wtime();

    //b (broadcast) and c_local (gathered) are needed on every rank
    double (*a)[n] = NULL;
    double (*b)[n] = malloc(sizeof(double[n][n]));
    double (*c_local)[n] = malloc(sizeof(double[local_rows][n]));
    double (*a_local)[n] = malloc(sizeof(double[local_rows][n]));

    if (rank == 0){
        a = malloc(sizeof(double[n][n]));   //allocated only on rank 0

        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                a[i][j] = 2.0;
    }

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            b[i][j] = 3.0;

    for (i = 0; i < local_rows; i++)
        for (j = 0; j < n; j++)
            c_local[i][j] = 0.0;

    //scatter rows of a: each process gets local_rows * n doubles (variable per rank)
    MPI_Scatterv((rank == 0 ? a : NULL), sendcounts, displs, MPI_DOUBLE, a_local, local_rows * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //every process needs b
    MPI_Bcast(b, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (i = 0; i < local_rows; ++i)
        for (k = 0; k < n; k++)
            for (j = 0; j < n; ++j)
                c_local[i][j] += a_local[i][k] * b[k][j];

    double (*c)[n] = NULL;

    if (rank == 0)
        c = malloc(sizeof(double[n][n]));

    //gather variable-sized row strips back into c
    MPI_Gatherv(c_local, local_rows * n, MPI_DOUBLE, (rank == 0 ? c : NULL), sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0){
        FILE *f = fopen("mat-res.txt", "w");

        if (!f){
            perror("fopen");
            MPI_Finalize();
            return 1;
        }

        fprintf(f, "%d\n\n", n);

        for (int ii = 0; ii < 1000; ii++){
            for (int jj = 0; jj < 1000; jj++)
                fprintf(f, "%.0f ", c[ii][jj]);

            fprintf(f, "\n");
        }

        fclose(f);
        free(a);
        free(c);
        free(sendcounts);
        free(displs);
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