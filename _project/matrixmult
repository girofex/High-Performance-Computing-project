#define n 5000

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc,char **argv) {
  int i, j, k;

  double ( *a )[n] = malloc(sizeof(double[n][n]));
  double ( *b )[n] = malloc(sizeof(double[n][n]));
  double ( *c )[n] = malloc(sizeof(double[n][n]));

  for (i=0; i<n; i++)
     for (j=0; j<n; j++) {
        a[i][j] = 2.0;
        b[i][j] = 3.0;
        c[i][j] = 0.0;
     }

  for (i=0; i<n; ++i)
     for (k=0; k<n; k++)
        for (j=0; j<n; ++j)
           c[i][j] += a[i][k]*b[k][j];



  FILE *f = fopen("mat-res.txt", "w");
  if (!f) {
     perror("fopen");
      return 1;
  }

  fprintf(f, "%d\n\n", n);  
  for (int i = 0; i < 1000; i++) {
     for (int j = 0; j < 1000; j++) {
        fprintf(f, "%.0f ", c[i][j]);
     }
     fprintf(f, "\n");
  }

  fclose(f);

  free(a);
  free(b);
  free(c);
  return 0;
}