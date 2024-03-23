#ifndef UTILS_H
#define UTILS_H

#define N_SPECIES (9)

#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char *gen_initial_grid(long long N, float density, int seed, int me,
                                int nprocs, int grid[1], MPI_Comm cart_comm);

void writeBorders(unsigned char *grid, long long paddingSize, long long x,
                  long long y, long long z, unsigned char value);

#endif // UTILS_H
