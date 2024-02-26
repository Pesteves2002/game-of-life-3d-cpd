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
        int index = z * N * N + y * N + x;
        updateNeighborsCount(cube->cache, cube->side_size, x, y, z,
                             cube->grid[index] == 0 ? 0 : 1);
      }
    }
  }

  return cube;
}

void updateNeighborsCount(unsigned char *cache, long long size, int x, int y,
                          int z, unsigned char value) {
  for (int k = -1; k <= 1; k++) {
    int z_ = (z + k + size) % size * size * size;
    for (int j = -1; j <= 1; j++) {
      int y_ = (y + j + size) % size * size;
      for (int i = -1; i <= 1; i++) {
        int x_ = (x + i + size) % size;
        if (i == 0 && j == 0 && k == 0) {
          continue;
        }
        cache[z_ + y_ + x_] += value;
      }
    }
  }
};
