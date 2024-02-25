#ifndef UTILS_H
#define UTILS_H

#define N_SPECIES (9)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char leftState : 4;
  unsigned char rightState : 4;
} Cell;

typedef struct {
  unsigned char aliveNeighbours;
} Cache;

typedef struct {
  unsigned long long side_size;
  Cell *grid;
  Cache *cache;
} Cube;

#define GET_CELL(cube, x, y, z)                                                \
  cube->grid[x + cube->side_size * (y + cube->side_size * z)]

#define GET_NEIGHBOUR(cube, x, y, z)                                           \
  cube->cache[x + cube->side_size * (y + cube->side_size * z)]

#define SET_CELL_LEFT_STATE(cube, x, y, z, value)                              \
  GET_CELL(cube, x, y, z).leftState = value;

#define SET_CELL_RIGHT_STATE(cube, x, y, z, value)                             \
  GET_CELL(cube, x, y, z).rightState = value;

Cube *gen_initial_grid(long long N, float density, int seed);

void updateNeighborsCount(Cache *cache, long long size, int x, int y, int z,
                          unsigned char value);

#endif // UTILS_H
