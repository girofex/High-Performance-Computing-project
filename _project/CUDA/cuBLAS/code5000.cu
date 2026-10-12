
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <cublas_v2.h>

#define M 5000
#define N 5000
#define K 5000

// Macro for CUDA error checking
#define cudaCheckError() { \
    cudaError_t err = cudaGetLastError(); \
    if (err != cudaSuccess) { \
        fprintf(stderr, "CUDA error at %s:%d: %s\n", __FILE__, __LINE__, cudaGetErrorString(err)); \
        exit(EXIT_FAILURE); \
    } \
}

int main(void) {
    cudaEvent_t start, stop, start_dev, stop_dev;
    float elapsed_time_ms_kernel, elapsed_time_ms_total;

    cudaEventCreate(&start); cudaCheckError();
    cudaEventCreate(&stop); cudaCheckError();
    cudaEventCreate(&start_dev); cudaCheckError();
    cudaEventCreate(&stop_dev); cudaCheckError();

    cublasHandle_t handle;
    cublasCreate(&handle);

    int i, j;

   // allocation on host
   double *h_A = (double *)malloc(M * K * sizeof(double));
   double *h_B = (double *)malloc(K * N * sizeof(double));
   double *h_C = (double *)malloc(M * N * sizeof(double));

   if (h_A == NULL || h_B == NULL || h_C == NULL) {
       fprintf(stderr, "Host memory allocation failed!\n");
       exit(EXIT_FAILURE);
   }

   // initialization
   for (i = 0; i < M; i++)
      for (j = 0; j < N; j++){
         h_A[i * N + j] = 2.0;
         h_B[i * N + j] = 3.0;
         h_C[i * N + j] = 0.0;
      }

    const double alpha = 1.0;
    const double beta  = 0.0;

    cudaEventRecord(start_dev, 0); cudaCheckError();
    // Device memory pointers
    double *d_A, *d_B, *d_C;
    cudaMalloc((void**)&d_A, sizeof(double) * M * K);
    cudaMalloc((void**)&d_B, sizeof(double) * K * N);
    cudaMalloc((void**)&d_C, sizeof(double) * M * N);

    // Transfer data from host to device (Column-Major format used by cuBLAS)
    cudaMemcpy(d_A, h_A, sizeof(double) * M * K, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, sizeof(double) * K * N, cudaMemcpyHostToDevice);

    // Perform DGEMM: C = alpha * A * B + beta * C
    // Note: cuBLAS uses column-major storage by default.
    cudaEventRecord(start, 0); cudaCheckError();
    cublasDgemm(handle,
                CUBLAS_OP_N, CUBLAS_OP_N,
                M, N, K,
                &alpha,
                d_A, M,
                d_B, K,
                &beta,
                d_C, M);

    cudaDeviceSynchronize(); cudaCheckError(); // Ensure kernel completes before measuring time

    cudaEventRecord(stop, 0); cudaCheckError();
    cudaEventSynchronize(stop); cudaCheckError();

    // Transfer result back to host
    cudaMemcpy(h_C, d_C, sizeof(double) * M * N, cudaMemcpyDeviceToHost);

    cudaEventRecord(stop_dev, 0); cudaCheckError();
    cudaEventSynchronize(stop_dev); cudaCheckError();

    cudaEventElapsedTime(&elapsed_time_ms_kernel, start, stop); cudaCheckError();
    cudaEventElapsedTime(&elapsed_time_ms_total, start_dev, stop_dev); cudaCheckError();

    printf("Result example: c[0][0] = %f\n", h_C[0]);
    printf("Time to calculate results (kernel only): %f ms.\n", elapsed_time_ms_kernel);
    printf("Total time with I/O (H2D and D2H copies): %f ms.\n", elapsed_time_ms_total);

    // Cleanup
    cudaFree(d_A); cudaCheckError();
    cudaFree(d_B); cudaCheckError();
    cudaFree(d_C); cudaCheckError();
    cublasDestroy(handle);

    free(h_A);
    free(h_B);
    free(h_C);

    return 0;
}
