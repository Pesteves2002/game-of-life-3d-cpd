#ifndef UTILS_H
#define UTILS_H

#define N_SPECIES (9)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned long long side_size;
  unsigned char *grid;
  unsigned char *cache;
} Cube;

#define GET_CELL(cube, x, y, z)                                                \
  cube->grid[x + cube->side_size * (y + cube->side_size * z)]

#define GET_CELL_INDEX(cube, index) cube->grid[index]

#define GET_NEIGHBOUR(cube, x, y, z)                                           \
  cube->cache[x + cube->side_size * (y + cube->side_size * z)]

Cube *gen_initial_grid(long long N, float density, int seed);

void updateNeighborsCount(unsigned char *cache, long long size, int x, int y,
                          int z, unsigned char value);

#endif // UTILS_H
