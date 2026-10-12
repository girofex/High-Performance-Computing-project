
#define n 15000

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

__global__ void gpuMatMul(double *a, double *b, double *c){
   int row = blockIdx.y * blockDim.y + threadIdx.y;
   int col = blockIdx.x * blockDim.x + threadIdx.x;

   if (row < n && col < n) { // Add bounds check for safety
       double sum = 0.0;
       for (int i = 0; i < n; i++)
          sum += a[row * n + i] * b[i * n + col];
       c[row * n + col] = sum;
   }
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

   // allocation on host
   double *a = (double *)malloc(n * n * sizeof(double));
   double *b = (double *)malloc(n * n * sizeof(double));
   double *c = (double *)malloc(n * n * sizeof(double));

   if (a == NULL || b == NULL || c == NULL) {
       fprintf(stderr, "Host memory allocation failed!\n");
       exit(EXIT_FAILURE);
   }

   // initialization
   for (i = 0; i < n; i++)
      for (j = 0; j < n; j++){
         a[i * n + j] = 2.0;
         b[i * n + j] = 3.0;
         c[i * n + j] = 0.0;
      }

   cudaEventRecord(start_dev, 0); cudaCheckError();
   // allocation on device
   double *a_dev, *b_dev, *c_dev;
   cudaMalloc((void **)&a_dev, n * n * sizeof(double)); cudaCheckError();
   cudaMalloc((void **)&b_dev, n * n * sizeof(double)); cudaCheckError();
   cudaMalloc((void **)&c_dev, n * n * sizeof(double)); cudaCheckError();

   // copy data from host to device
   cudaMemcpy(a_dev, a, n * n * sizeof(double), cudaMemcpyHostToDevice); cudaCheckError();
   cudaMemcpy(b_dev, b, n * n * sizeof(double), cudaMemcpyHostToDevice); cudaCheckError();

   dim3 dimGrid(ceil((double)n / 32.0), ceil((double)n / 32.0), 1);
   dim3 dimBlock(32, 32, 1);

   // Print N value for confirmation
   printf("N value: %d\n", n);
   printf("Launching kernel with dimGrid(%d, %d, %d) and dimBlock(%d, %d, %d)\n",
          dimGrid.x, dimGrid.y, dimGrid.z, dimBlock.x, dimBlock.y, dimBlock.z);

   cudaEventRecord(start, 0); cudaCheckError();

   gpuMatMul<<<dimGrid, dimBlock>>>(a_dev, b_dev, c_dev); cudaCheckError();
   cudaDeviceSynchronize(); cudaCheckError(); // Ensure kernel completes before measuring time

   cudaEventRecord(stop, 0); cudaCheckError();
   cudaEventSynchronize(stop); cudaCheckError();

   // data from device to host
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
