#ifndef UTILS_H
#define UTILS_H

#define N_SPECIES (9)

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char *gen_initial_grid(long long N, float density, int seed, int me,
                                int nprocs, int grid[3], MPI_Comm cart_comm);

void writeBorders(unsigned char *grid, int paddingSize, int x, int y, int z,
                  unsigned char value);

void updateNeighborsCount(unsigned char *cache, long long size, int x, int y,
                          int z, unsigned char value);

#endif // UTILS_H
