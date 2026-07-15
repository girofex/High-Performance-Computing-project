/* The following code computes the interaction forces of N point charges at a set potential V and periodic boundary conditions.
On Aulaweb are two different input files named "position.xyz.DIM" . In order to choose what file you are accessing to, remember to define the DIM parameter in compilation phase, for example:
gcc -O3 -DDIM=55000 Nbody.c -o nbody -lm
After the execution, an output file with the results will be produced. It will contain the total energy of the system at the first line, and then the forces interacting with each particle.
Try to parallelize the code keeping in mind the following variations:
- while parallelizing the for loop you can choose between different scheduling options (static, guided, dynamic, ...) make some tests and see the differences.
- pay attention to the update of the "forces" array. You may want to update it atomically or by reducing it. Test both options and see the differences.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
int main() {
	char fn[FILENAME_MAX];
	FILE *fin, *fout;
	double (*pos)[3], (*forces)[3];
	double rij[3], d, d2, d3, ene, cut2=1000.0;
	unsigned i, j, k, nbodies=DIM;
	sprintf(fn, "positions.xyz.%u", nbodies);
	fin = fopen(fn, "r");

        if (fin == NULL) {
                perror(fn);
                exit(-1);
        }

	pos = calloc(nbodies, sizeof(*pos));
	forces = calloc(nbodies, sizeof(*forces));
        if (pos == NULL || forces == NULL) {
                fprintf(stderr, "Not enough memory!\n");
                exit(-2);
        }        

	for(i=0; i<nbodies; ++i)
        	fscanf(fin, "%lf%lf%lf", pos[i]+0, pos[i]+1, pos[i]+2);

	fclose(fin);

	ene = 0.0;

	for(i=0; i<nbodies; ++i)
		for(j=i+1; j<nbodies; ++j) {
			d2 = 0.0;
			for(k=0; k<3; ++k) {
				rij[k] = pos[i][k] - pos[j][k];
				d2 += rij[k]*rij[k];
			}
                        if (d2 <= cut2) {
                           d = sqrt(d2);
                           d3 = d*d2;
			   for(k=0; k<3; ++k) {
                                double f = -rij[k]/d3;
				forces[i][k] += f;
				forces[j][k] -= f;
			   }
			   ene += -1.0/d; 
                        }
		}
        // saving results to file
        fout = fopen("results", "w");
        if (fout == NULL) {
          perror("results");
          exit(-1);
        }

	fprintf(fout, "%20.10lE\n", ene);
	for(i=0; i<nbodies; ++i) {
		fprintf(fout, "%5d ", i);
		for(j=0; j<3; ++j)
			fprintf(fout,"%20.10lE", forces[i][j]);
		fprintf(fout,"\n");
	}

	return 0;
}