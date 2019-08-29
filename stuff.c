#include "stuff.h"

void sw_iterate() {
	reset_lattice();
	int i,j,k;
	/* 
	 * STEP 1: SET UP BOND CONFIGURATION 
	 */
	for(i = 0; i < x_size; ++i) 
	for(j = 0; j < y_size; ++j) 
	for(k = 0; k < z_size; ++k) {
		// positive x direction
		if(lattice[i][j][k].spin == lattice[i+1][j][k].spin && (double)rand() / (double)RAND_MAX < prob)
			lattice[i][j][k].bond_x = true;
		// positive y direction
		if(lattice[i][j][k].spin == lattice[i][j+1][k].spin && (double)rand() / (double)RAND_MAX < prob)
			lattice[i][j][k].bond_y = true;
		// positive z direction
		if(k == z_size - 1) {
			if(lattice[i][j][k].spin == lattice[i][j][0].spin && (double)rand() / (double)RAND_MAX < prob)
				lattice[i][j][k].bond_z = true;
		}
		else {
			if(lattice[i][j][k].spin == lattice[i][j][k+1].spin && (double)rand() / (double)RAND_MAX < prob)
				lattice[i][j][k].bond_z = true;
		}
	}
	
	/*
	 * STEP 2: GIVE EACH SITE A LABEL FOR WHICH CLUSTER IT'S IN 
	 * Each site initially has a unique label.
	 * Bonded nearest neighbours have their labels set to be the minimum of the two of them.
	 * This is repeated until every cluster has all of its sites having the same label.
	 */	
	bool finished = false;
	while(!finished) {
		finished = true;
		for(i = 0; i < x_size; ++i)
		for(j = 0; j < y_size; ++j)
		for(k = 0; k < z_size; ++k) {
			if(lattice[i][j][k].bond_x) {
				if(lattice[i][j][k].label > lattice[i+1][j][k].label) {
					lattice[i][j][k].label = lattice[i+1][j][k].label;
					finished = false;
				}
				else if(lattice[i+1][j][k].label > lattice[i][j][k].label) {
					lattice[i+1][j][k].label = lattice[i][j][k].label;
					finished = false;
				}
			}
			if(lattice[i][j][k].bond_y) {
				if(lattice[i][j][k].label > lattice[i][j+1][k].label) {
					lattice[i][j][k].label = lattice[i][j+1][k].label;
					finished = false;
				}
				else if(lattice[i][j+1][k].label > lattice[i][j][k].label) {
					lattice[i][j+1][k].label = lattice[i][j][k].label;
					finished = false;
				}
			}
			if(lattice[i][j][k].bond_z) {
				if(k == z_size - 1) {
					if(lattice[i][j][k].label > lattice[i][j][0].label) {
						lattice[i][j][k].label = lattice[i][j][0].label;
						finished = false;
					}
					else if(lattice[i][j][0].label > lattice[i][j][k].label) {
						lattice[i][j][0].label = lattice[i][j][k].label;
						finished = false;
					}
				}
				else {
					if(lattice[i][j][k].label > lattice[i][j][k+1].label) {
						lattice[i][j][k].label = lattice[i][j][k+1].label;
						finished = false;
					}
					else if(lattice[i][j][k+1].label > lattice[i][j][k].label) {
						lattice[i][j][k+1].label = lattice[i][j][k].label;
						finished = false;
					}
				}
			}
		}
	}
	/*
	 * STEP 3: Make an array the size of the lattice. For each initial label, generate a random spin. 
	 * Flip each spin to the random spin at the point of this array corresponding to its cluster label.
	 */
	for(i = 0; i < x_size * y_size * z_size; ++i)
		rand_spins[i] = rand() % q;
	// flip every spin to a random value from 0...q-1, based on its cluster label
	for(i = 0; i < x_size; ++i) 
	for(j = 0; j < y_size; ++j) 
	for(k = 0; k < z_size; ++k)
		lattice[i][j][k].spin = rand_spins[ lattice[i][j][k].label ];
}


void magnetization() {
	int i, j, k;
	double x_sum = 0, y_sum = 0;
	for(i = 0; i < x_size; ++i)
	for(j = 0; j < y_size; ++j)
	for(k = 0; k < z_size; ++k) {
		x_sum += x_values[ lattice[i][j][k].spin ];
		y_sum += y_values[ lattice[i][j][k].spin ];
	}
	x_sum /= x_size * y_size * z_size;
	y_sum /= x_size * y_size * z_size;
	mag = sqrt(x_sum * x_sum + y_sum * y_sum);
}

// Builds in periodic boundary conditions in x direction and y direction
// each site's spin is set to a random value from 0...q-1
// each site is given a unique label from 0...n_sites-1
// the bonds are not initialised to either true or false 
void init_lattice() {
	point ***offset_lattice = malloc( (x_size + 2) * sizeof(point**) );
	lattice = &offset_lattice[1];
	point **lattice_row;
	point **offset_lattice_row;
	point *lattice_data = malloc(x_size*y_size*z_size * sizeof(point) );
	int i, j, k;
	for(i = 0; i < x_size; ++i) {
		offset_lattice_row = malloc( (y_size+2) * sizeof(point*) );
		lattice_row = &offset_lattice_row[1];
		lattice[i] = &lattice_row[0];
		for(j = 0; j < y_size; ++j) { 
			lattice_row[j] = &lattice_data[i * (y_size * z_size) + j * z_size];
			for(k = 0; k < z_size; ++k) { 
				lattice[i][j][k].spin = rand() % q;
				lattice[i][j][k].label = i * (y_size * z_size) + j * z_size + k;
			} 
		}
		lattice_row[-1] = lattice_row[y_size - 1];
		lattice_row[y_size] = lattice_row[0];
	}
	lattice[-1] = lattice[x_size - 1];
	lattice[x_size] = lattice[0];
}

void print_lattice(point ***lattice) {
	int i,j,k;
	for(i = 0; i < x_size; ++i) { 
		printf("\n ==== ROW %i ====\n", i);
		for(j = 0; j < y_size; ++j) { 
			for(k = 0; k < z_size; ++k) { 
				printf("%i ", lattice[i][j][k].spin);
			}
			printf("\n");
		}
	}
}

void print_lattice_labels(point ***lattice) {
	int i,j,k;
	for(i = 0; i < x_size; ++i) { 
		printf("\n ==== ROW %i LABELS ====\n", i);
		for(j = 0; j < y_size; ++j) { 
			for(k = 0; k < z_size; ++k) { 
				printf("%i ", lattice[i][j][k].label);
			} 
			printf("\n");
		} 
	} 
}

void randomize_lattice() {
	int i,j,k;
	for(i = 0; i < x_size; ++i)
	for(j = 0; j < y_size; ++j)
	for(k = 0; k < z_size; ++k) {
		lattice[i][j][k].spin = rand() % q;
		lattice[i][j][k].label = i * (y_size * z_size) + j * z_size + k;
		lattice[i][j][k].bond_x = false;
		lattice[i][j][k].bond_y = false;
		lattice[i][j][k].bond_z = false;
	}
}
void reset_lattice() {
	int i,j,k;
	for(i = 0; i < x_size; ++i)
	for(j = 0; j < y_size; ++j)
	for(k = 0; k < z_size; ++k) {
		lattice[i][j][k].label = i * (y_size * z_size) + j * z_size + k;
		lattice[i][j][k].bond_x = false;
		lattice[i][j][k].bond_y = false;
		lattice[i][j][k].bond_z = false;
	}
}

void free_lattice() {
	free(&lattice[0][0][0]); // free the data
	int i;
	for(i=0; i < x_size; ++i)
		free(&lattice[i][-1]); // free the arrays of pointers to the columns;
	free(&lattice[-1]); // free the array of pointers to the rows
}
