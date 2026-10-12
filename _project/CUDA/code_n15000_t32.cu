
#define n 15000
#define TILE 32

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda_runtime.h>

// Macro for CUDA error checking
#define cudaCheckError() { \
    cudaError_t err = cudaGetLastError(); \
    if (err != cudaSuccess) { \
        fprintf(stderr, "CUDA error at %s:%d: %s\n", __FILE__, __LINE__, cudaGetErrorString(err)); \
        exit(EXIT_FAILURE); \
    } \
}

__global__ void gpuMatMulTiled(double *a, double *b, double *c){
    __shared__ double As[TILE][TILE];
    __shared__ double Bs[TILE][TILE];

    int row = blockIdx.y * TILE + threadIdx.y;
    int col = blockIdx.x * TILE + threadIdx.x;

    double sum = 0.0;

    int numTiles = (n + TILE - 1) / TILE;

    for (int t = 0; t < numTiles; t++) {
        int aCol = t * TILE + threadIdx.x;
        int bRow = t * TILE + threadIdx.y;

        As[threadIdx.y][threadIdx.x] = (row < n && aCol < n) ? a[row * n + aCol] : 0.0;
        Bs[threadIdx.y][threadIdx.x] = (bRow < n && col < n) ? b[bRow * n + col] : 0.0;

        __syncthreads();

        for (int k = 0; k < TILE; k++)
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];

        __syncthreads();
    }

    if (row < n && col < n)
        c[row * n + col] = sum;
}

int main(int argc, char **argv)
{
   cudaEvent_t start, stop, start_dev, stop_dev;
   float elapsed_time_ms_kernel, elapsed_time_ms_total;

   cudaEventCreate(&start); cudaCheckError();
   cudaEventCreate(&stop); cudaCheckError();
   cudaEventCreate(&start_dev); cudaCheckError();
   cudaEventCreate(&stop_dev); cudaCheckError();

   int i, j;

   double *a = (double *)malloc(n * n * sizeof(double));
   double *b = (double *)malloc(n * n * sizeof(double));
   double *c = (double *)malloc(n * n * sizeof(double));

   if (a == NULL || b == NULL || c == NULL) {
       fprintf(stderr, "Host memory allocation failed!\n");
       exit(EXIT_FAILURE);
   }

   for (i = 0; i < n; i++)
      for (j = 0; j < n; j++){
         a[i * n + j] = 2.0;
         b[i * n + j] = 3.0;
         c[i * n + j] = 0.0;
      }

   cudaEventRecord(start_dev, 0); cudaCheckError();
   double *a_dev, *b_dev, *c_dev;
   cudaMalloc((void **)&a_dev, n * n * sizeof(double)); cudaCheckError();
   cudaMalloc((void **)&b_dev, n * n * sizeof(double)); cudaCheckError();
   cudaMalloc((void **)&c_dev, n * n * sizeof(double)); cudaCheckError();

   cudaMemcpy(a_dev, a, n * n * sizeof(double), cudaMemcpyHostToDevice); cudaCheckError();
   cudaMemcpy(b_dev, b, n * n * sizeof(double), cudaMemcpyHostToDevice); cudaCheckError();

   dim3 dimGrid(ceil((double)n / TILE), ceil((double)n / TILE), 1);
   dim3 dimBlock(TILE, TILE, 1);

   printf("N value: %d\n", n);
   printf("TILE value: %d\n", TILE);
   printf("Launching kernel with dimGrid(%d, %d, %d) and dimBlock(%d, %d, %d)\n",
          dimGrid.x, dimGrid.y, dimGrid.z, dimBlock.x, dimBlock.y, dimBlock.z);

   cudaEventRecord(start, 0); cudaCheckError();

   gpuMatMulTiled<<<dimGrid, dimBlock>>>(a_dev, b_dev, c_dev); cudaCheckError();
   cudaDeviceSynchronize(); cudaCheckError();

   cudaEventRecord(stop, 0); cudaCheckError();
   cudaEventSynchronize(stop); cudaCheckError();

   cudaMemcpy(c, c_dev, n * n * sizeof(double), cudaMemcpyDeviceToHost); cudaCheckError();

   cudaEventRecord(stop_dev, 0); cudaCheckError();
   cudaEventSynchronize(stop_dev); cudaCheckError();

   cudaEventElapsedTime(&elapsed_time_ms_kernel, start, stop); cudaCheckError();
   cudaEventElapsedTime(&elapsed_time_ms_total, start_dev, stop_dev); cudaCheckError();

   printf("Result example: c[0][0] = %f\n", c[0]);
   printf("Time to calculate results (kernel only): %f ms.\n", elapsed_time_ms_kernel);
   printf("Total time with I/O (H2D and D2H copies): %f ms.\n", elapsed_time_ms_total);

   cudaFree(a_dev); cudaCheckError();
   cudaFree(b_dev); cudaCheckError();
   cudaFree(c_dev); cudaCheckError();

   free(a);
   free(b);
   free(c);

   return 0;
}
