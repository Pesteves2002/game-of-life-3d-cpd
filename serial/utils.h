#ifndef UTILS_H
#define UTILS_H

#define N_SPECIES (9)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned long long side_size;
  unsigned char *grid;
  unsigned char *neighbourCount;
} Cube;

#define CALC_INDEX(x, y, z, size) ((z) * (size) * (size) + (y) * (size) + (x))

Cube *gen_initial_grid(long long N, float density, int seed);

void updateNeighborsCount(unsigned char *cache, long long size, int x, int y,
                          int z, unsigned char value);

#endif // UTILS_H
