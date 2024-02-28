#include "utils.h"
#include <stdbool.h>

unsigned int seed;

void init_r4uni(int input_seed) { seed = input_seed + 987654321; }

float r4_uni() {
  int seed_in = seed;

  seed ^= (seed << 13);
  seed ^= (seed >> 17);
  seed ^= (seed << 5);

  return 0.5 + 0.2328306e-09 * (seed_in + (int)seed);
}

Cube *gen_initial_grid(long long N, float density, int input_seed) {
  Cube *cube = (Cube *)malloc(sizeof(Cube));
  if (cube == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  cube->side_size = N;
  cube->grid = (unsigned char *)calloc(N * N * N, sizeof(unsigned char));
  cube->cache = (unsigned char *)calloc(N * N * N, sizeof(unsigned char));
  if (cube->grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);
  for (unsigned long long x = 0; x < N * N * N; x++) {
    unsigned char state =
        r4_uni() < density ? (int)(r4_uni() * N_SPECIES) + 1 : 0;
    cube->grid[x] = state;
  }

  for (int z = 0; z < N; z++) {
    for (int y = 0; y < N; y++) {
      for (int x = 0; x < N; x++) {
        int index = CALC_INDEX(x, y, z, N);
        updateNeighborsCount(cube->cache, cube->side_size, x, y, z,
                             cube->grid[index] == 0 ? 0 : 1);
      }
    }
  }

  return cube;
}

void updateNeighborsCount(unsigned char *cache, long long size, int x, int y,
                          int z, unsigned char value) {
  int z1 = (z + size - 1) % size * size * size;
  int z2 = (z + size) % size * size * size;
  int z3 = (z + 1) % size * size * size;
  int y1 = (y + size - 1) % size * size;
  int y2 = (y + size) % size * size;
  int y3 = (y + 1) % size * size;
  int x1 = (x + size - 1) % size;
  int x2 = (x + size) % size;
  int x3 = (x + 1) % size;

  cache[z1 + y1 + x1] += value;
  cache[z1 + y1 + x2] += value;
  cache[z1 + y1 + x3] += value;
  cache[z1 + y2 + x1] += value;
  cache[z1 + y2 + x2] += value;
  cache[z1 + y2 + x3] += value;
  cache[z1 + y3 + x1] += value;
  cache[z1 + y3 + x2] += value;
  cache[z1 + y3 + x3] += value;

  cache[z2 + y1 + x1] += value;
  cache[z2 + y1 + x2] += value;
  cache[z2 + y1 + x3] += value;
  cache[z2 + y2 + x1] += value;
  // cache[z2 + y2 + x2] += value;
  cache[z2 + y2 + x3] += value;
  cache[z2 + y3 + x1] += value;
  cache[z2 + y3 + x2] += value;
  cache[z2 + y3 + x3] += value;

  cache[z3 + y1 + x1] += value;
  cache[z3 + y1 + x2] += value;
  cache[z3 + y1 + x3] += value;
  cache[z3 + y2 + x1] += value;
  cache[z3 + y2 + x2] += value;
  cache[z3 + y2 + x3] += value;
  cache[z3 + y3 + x1] += value;
  cache[z3 + y3 + x2] += value;
  cache[z3 + y3 + x3] += value;
};
