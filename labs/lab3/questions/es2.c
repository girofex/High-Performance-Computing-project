/*The following code determines the value of PI, by calculating an integral between 0 and 1. The integral is approximated as a sum of n intervals.
Parallelize the code with OpenMP.
Try also to solve the exercise without using the reduction clause.
*/



/* Exercise: Pi
 *
 * In this exercise you will determine the value 
 * of PI using the integral  of 
 *    4/(1+x*x) between 0 and 1. 
 *
 * The integral is approximated by a sum of n intervals.
 * 
 * The approximation to the integral in each interval is:
 *    (1/n)*4/(1+x*x). 
 */
#include <stdio.h>
#include <time.h>
               
#define PI25DT 3.141592653589793238462643

#define INTERVALS 10000000000

int main(int argc, char **argv)
{
    long int i, intervals = INTERVALS;
    double x, dx, f, sum, pi;
    double time2;
    
    time_t time1 = clock();

    printf("Number of intervals: %ld\n", intervals);

    sum = 0.0;
    dx = 1.0 / (double) intervals;

    for (i = 1; i <= intervals; i++) {
        x = dx * ((double) (i - 0.5));
        f = 4.0 / (1.0 + x*x);
        sum = sum + f;
    }
         
    pi = dx*sum;
    //Pay attention: it will sum the times of all threads
    time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

    printf("Computed PI %.24f\n", pi);
    printf("The true PI %.24f\n\n", PI25DT);
    printf("Elapsed time (s) = %.2lf\n", time2);

    return 0;
}