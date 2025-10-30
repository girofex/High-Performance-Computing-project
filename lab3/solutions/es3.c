// ES 3.1
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main() {
   
   double *T, *Tnew, *Tmp;
   double tol, var = DBL_MAX, top = 100.0;
   unsigned n, n2, maxIter, i, j, iter = 0;
   int itemsread;
   FILE *fout;

   printf("Enter mesh size, max iterations and tolerance: ");
   itemsread = scanf("%u ,%u ,%lf", &n, &maxIter, &tol);

   if (itemsread!=3) {
      fprintf(stderr, "Input error!\n");
      exit(-1);
   }

#ifdef _OPENMP 
    double startTime = omp_get_wtime();
#else
    time_t startTime = clock();
#endif

   n2 = n+2;
   T = calloc(n2*n2, sizeof(*T));
   Tnew = calloc(n2*n2, sizeof(*T));

   if (T == NULL || Tnew == NULL) {
      fprintf(stderr, "Not enough memory!\n");
      exit(EXIT_FAILURE);
   }

   // set boundary conditions

   for (i=1; i<=n; i++) {
      T[(n+1)*n2+i] = Tnew[(n+1)*n2+i] = i * top / (n+1);  
      T[i*n2+n+1] = Tnew[i*n2+n+1] = i * top / (n+1);
   }

   while(var > tol && iter <= maxIter) {
      ++iter;
      var = 0.0;
#pragma omp parallel for private(j) reduction(max:var)
   {
        for (i=1; i<=n; ++i)
           for (j=1; j<=n; ++j) {         
             Tnew[i*n2+j] = 0.25*( T[(i-1)*n2+j] + T[(i+1)*n2+j] 
                                + T[i*n2+(j-1)] + T[i*n2+(j+1)] );
        
             var = fmax(var, fabs(Tnew[i*n2+j] - T[i*n2+j]));
          }

  }

  Tmp=T; T=Tnew; Tnew=Tmp;

   if (iter%100 == 0)
      printf("iter: %8u, variation = %12.4lE\n", iter, var);

   }

#ifdef _OPENMP
    double endTime = omp_get_wtime() - startTime;
#else
    double endTime = (clock() - startTime) / (double) CLOCKS_PER_SEC;
#endif

   printf("Elapsed time (s) = %.2lf\n", endTime);
   printf("Mesh size: %u\n", n);
   printf("Stopped at iteration: %u\n", iter);
   printf("Maximum error: %lE\n", var);

   // saving results to file
   fout = fopen("results", "w");
   if (fout == NULL) {
      perror("results");
      exit(-1);
   }
   for (i=1; i<=n; ++i)
      for (j=1; j<=n; ++j)
         fprintf(fout, "%8u %8u %18.9lE\n", i, j, T[i*n2+j]);
   fclose(fout);
   free(T);
   free(Tnew);
   return 0;
}


// ES 3.2
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main() {
   
   double *T, *Tnew, *Tmp;
   double tol, var = DBL_MAX, top = 100.0;
   unsigned n, n2, maxIter, i, j, iter = 0;
   int itemsread;
   FILE *fout;

   printf("Enter mesh size, max iterations and tolerance: ");
   itemsread = scanf("%u ,%u ,%lf", &n, &maxIter, &tol);

   if (itemsread!=3) {
      fprintf(stderr, "Input error!\n");
      exit(-1);
   }

#ifdef _OPENMP 
    double startTime = omp_get_wtime();
#else
    time_t startTime = clock();
#endif

   n2 = n+2;
   T = calloc(n2*n2, sizeof(*T));
   Tnew = calloc(n2*n2, sizeof(*T));

   if (T == NULL || Tnew == NULL) {
      fprintf(stderr, "Not enough memory!\n");
      exit(EXIT_FAILURE);
   }

   // set boundary conditions

   for (i=1; i<=n; i++) {
      T[(n+1)*n2+i] = Tnew[(n+1)*n2+i] = i * top / (n+1);  
      T[i*n2+n+1] = Tnew[i*n2+n+1] = i * top / (n+1);
   }
#pragma omp parallel
 {
   while(var > tol && iter <= maxIter) {
#pragma omp barrier
#pragma omp single 
  {
      ++iter;
      var = 0.0;
  }
#pragma omp for private(j) reduction(max:var)
      for (i=1; i<=n; ++i)
         for (j=1; j<=n; ++j) {         
            Tnew[i*n2+j] = 0.25*( T[(i-1)*n2+j] + T[(i+1)*n2+j] 
                                + T[i*n2+(j-1)] + T[i*n2+(j+1)] );
        
            var = fmax(var, fabs(Tnew[i*n2+j] - T[i*n2+j]));
         }

#pragma omp single nowait
  {
         Tmp=T; T=Tnew; Tnew=Tmp;

         if (iter%100 == 0)
            printf("iter: %8u, variation = %12.4lE\n", iter, var);

  }

   }
 }

#ifdef _OPENMP
    double endTime = omp_get_wtime() - startTime;
#else
    double endTime = (clock() - startTime) / (double) CLOCKS_PER_SEC;
#endif

   printf("Elapsed time (s) = %.2lf\n", endTime);
   printf("Mesh size: %u\n", n);
   printf("Stopped at iteration: %u\n", iter);
   printf("Maximum error: %lE\n", var);

   // saving results to file
   fout = fopen("results", "w");
   if (fout == NULL) {
      perror("results");
      exit(-1);
   }
   for (i=1; i<=n; ++i)
      for (j=1; j<=n; ++j)
         fprintf(fout, "%8u %8u %18.9lE\n", i, j, T[i*n2+j]);
   fclose(fout);
   free(T);
   free(Tnew);
   return 0;
}