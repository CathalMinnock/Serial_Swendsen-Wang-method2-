#include "stuff.h"
#include <time.h>
#include <unistd.h>

void init(int argc, char** argv) {
	srand(time(NULL));
	int option_index = 0;
	x_size = 8; y_size = 8; z_size = 8; 
	q = 2; beta = 0.4; filename = "apple.txt";
	samples = 1; steps_between_samples = 1;
	while((option_index = getopt(argc, argv, "x:y:z:q:b:s:f:a:" )) != -1) {
		switch(option_index) {
			case 'x':
				x_size = atoi(optarg);
				break;
			case 'y':
				y_size = atoi(optarg);
				break;
			case 'z':
				z_size = atoi(optarg);
				break;
			case 'q':
				q = atoi(optarg);
				break;
			case 'b':
				beta = atof(optarg);
				break;
			case 's':
				samples = atoi(optarg);
				break;
            case 'a':
                steps_between_samples = atoi(optarg);
                break;
			case 'f':
				filename = optarg;
				break;
			default:
				printf("Option incorrect\n");
				exit(1);
		}
	}
	prob = 1 - exp(-2 * beta);
	rand_spins = malloc(x_size * y_size * z_size * sizeof(unsigned char));
	x_values = malloc(q * sizeof(double));
	y_values = malloc(q * sizeof(double));
	int i;
	for(i=0; i < q; ++i) {
		x_values[i] = sin( (2 * M_PI * i ) / (double) q);
		y_values[i] = cos( (2 * M_PI * i ) / (double) q);
	}
	fp = fopen(filename, "w");
}
int main(int argc, char** argv)
{	
	init(argc, argv);
	int i, j;
	init_lattice();
	randomize_lattice();
	for(i = 0; i < 100; ++i)
		sw_iterate();
	for(i = 0; i < samples; ++i) {
		for(j = 0; j < steps_between_samples; ++j)
			sw_iterate();
		magnetization();
		fprintf(fp, "%lf\n", mag);
	}
	fclose(fp);
	free_lattice();
	free(rand_spins);
	return 0;
}
