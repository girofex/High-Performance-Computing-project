/*
Compile and run the following "Hello World" and experiment with the OMP_NUM_THREADS variable.
If you encounter any error during the execution, try to fix it!
*/

#include <stdio.h>
#ifdef _OPENMP 
#include<omp.h> 
#endif

int main(int argc, char* argv[])
{
#ifdef _OPENMP
int iam;
#pragma omp parallel
    { /* the parallel block starts here */
        iam=omp_get_thread_num();
#pragma omp critical
        printf("Hello from %d\n",iam);
    } /* the parallel block ends here */

#else
        printf("Hello, this is a serial program.\n");
#endif
    return 0;
}