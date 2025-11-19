#define n 10
//ATTENTION! THIS SHOULD BE 5000

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <time.h>
#ifdef _OPENMP
   #include <omp.h>
#endif

int main(int argc,char **argv) {
  int i, j, k;

  double ( *a )[n] = malloc(sizeof(double[n][n]));
  double ( *b )[n] = malloc(sizeof(double[n][n]));
  double ( *c )[n] = malloc(sizeof(double[n][n]));

#ifdef _OPENMP 
    double startTime = omp_get_wtime();
#else
    time_t startTime = clock();
#endif

  for (i=0; i<n; i++)
     for (j=0; j<n; j++) {
        a[i][j] = 2.0;
        b[i][j] = 3.0;
        c[i][j] = 0.0;
     }

  for (i=0; i<n; ++i)
     for (k=0; k<n; k++)
     //#pragma omp parallel for reduction(+: c[n][n])
     #pragma simd
        for (j=0; j<n; ++j) //CRITICAL
           c[i][j] += a[i][k]*b[k][j];

   #ifdef _OPENMP
      double endTime = omp_get_wtime() - startTime;
   #else
      double endTime = (clock() - startTime) / (double) CLOCKS_PER_SEC;
   # endif

  FILE *f = fopen("mat-res.txt", "w");
  if (!f) {
     perror("fopen");
  }

  fprintf(f, "%d\n\n", n);  
  for (int i = 0; i < 1000; i++) {
     for (int j = 0; j < 1000; j++) {
        fprintf(f, "%.0f ", c[i][j]);
     }
     fprintf(f, "\n");
  }
  printf("Elapsed time (s) = %.2lf\n", endTime);

  fclose(f);


  free(a);
  free(b);
  free(c);
  return 0;
}
