#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

unsigned int seed;

void init_r4uni(int input_seed) { seed = input_seed + 987654321; }

float r4_uni() {
  int seed_in = seed;

  seed ^= (seed << 13);
  seed ^= (seed >> 17);
  seed ^= (seed << 5);

  return 0.5 + 0.2328306e-09 * (seed_in + (int)seed);
}

Cell ***gen_initial_grid(long long N, float density, int input_seed) {
  int x, y, z;

  Cell ***grid = (Cell ***)malloc(N * sizeof(Cell **));
  if (grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }
  for (x = 0; x < N; x++) {
    grid[x] = (Cell **)malloc(N * sizeof(Cell *));
    if (grid[x] == NULL) {
      printf("Failed to allocate matrix\n");
      exit(1);
    }
    grid[x][0] = (Cell *)calloc(N * N, sizeof(Cell));
    if (grid[x][0] == NULL) {
      printf("Failed to allocate matrix\n");
      exit(1);
    }
    for (y = 1; y < N; y++)
      grid[x][y] = grid[x][0] + y * N;
  }

  init_r4uni(input_seed);
  for (x = 0; x < N; x++)
    for (y = 0; y < N; y++)
      for (z = 0; z < N; z++)
        if (r4_uni() < density)
          grid[x][y][z].rightState = (int)(r4_uni() * N_SPECIES) + 1;

  return grid;
}
