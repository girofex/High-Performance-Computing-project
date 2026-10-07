#define min(x, y) (((x) < (y)) ? (x) : (y))
#define LOOP_COUNT 10

#include <stdio.h>
#include <stdlib.h>
#include "mkl.h"

int main()
{
  double *A, *B, *C;
  int m, n, k, i, j;
  double alpha, beta;
  double s_initial, s_elapsed;
  int r;

  m = 15000, k = 15000, n = 15000;
  alpha = 1.0;
  beta = 0.0;

  A = (double *)mkl_malloc(m * k * sizeof(double), 64);
  B = (double *)mkl_malloc(k * n * sizeof(double), 64);
  C = (double *)mkl_malloc(m * n * sizeof(double), 64);

  if(A == NULL || B == NULL || C == NULL){
    printf("\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);

    return 1;
  }

  for (i = 0; i < (m * k); i++)
    A[i] = 2.0;

  for (i = 0; i < (k * n); i++)
    B[i] = 3.0;

  for (i = 0; i < (m * n); i++)
    C[i] = 0.0;

  s_initial = dsecnd();

  for (r = 0; r < LOOP_COUNT; r++)
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, k, B, n, beta, C, n);

  s_elapsed = (dsecnd() - s_initial) / LOOP_COUNT;
  printf(" Time: %.5f milliseconds \n\n", (s_elapsed * 1000));

  printf(" Top left corner of matrix A: \n");
  for (i = 0; i < min(m, 6); i++){
    for (j = 0; j < min(k, 6); j++)
      printf("%12.0f", A[j + i * k]);

    printf("\n");
  }

  printf("\n Top left corner of matrix B: \n");
  for (i = 0; i < min(k, 6); i++){
    for (j = 0; j < min(n, 6); j++)
      printf("%12.0f", B[j + i * n]);
      
    printf("\n");
  }

  printf("\n Top left corner of matrix C: \n");
  for (i = 0; i < min(m, 6); i++){
    for (j = 0; j < min(n, 6); j++)
      printf("%12.5G", C[j + i * n]);
      
    printf("\n");
  }

  mkl_free(A);
  mkl_free(B);
  mkl_free(C);

  return 0;
}