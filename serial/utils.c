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
  long long paddingSize = N + 2;
  cube->grid = (unsigned char *)calloc(paddingSize * paddingSize * paddingSize,
                                       sizeof(unsigned char));
  cube->cache = (unsigned char *)calloc(paddingSize * paddingSize * paddingSize,
                                        sizeof(unsigned char));
  if (cube->grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);

  // padding in 0 and paddingSize - 1
  for (int z = 0; z < paddingSize; z++) {
    for (int y = 0; y < paddingSize; y++) {
      for (int x = 0; x < paddingSize; x++) {
        int index = z * paddingSize * paddingSize + y * paddingSize + x;
        if (x == 0 || y == 0 || z == 0 || x == paddingSize - 1 ||
            y == paddingSize - 1 || z == paddingSize - 1) {
          continue;
        } else {
          unsigned char value =
              r4_uni() < density ? (int)(r4_uni() * N_SPECIES) + 1 : 0;
          cube->grid[index] = value;

          bool border_x = x == 1 || x == paddingSize - 2;
          bool border_y = y == 1 || y == paddingSize - 2;
          bool border_z = z == 1 || z == paddingSize - 2;

          int x_ = x == 1 ? paddingSize - 1 : 0;
          int y_ = y == 1 ? paddingSize - 1 : 0;
          int z_ = z == 1 ? paddingSize - 1 : 0;

          if (border_x) {
            index = z * paddingSize * paddingSize + y * paddingSize + x_;
            cube->grid[index] = value;
          }

	  if (border_y) {
	    index = z * paddingSize * paddingSize + y_ * paddingSize + x;
	    cube->grid[index] = value;
	  }

	  if (border_z) {
	    index = z_ * paddingSize * paddingSize + y * paddingSize + x;
	    cube->grid[index] = value;
	    }

	  if (border_x && border_y) {
	  index = z * paddingSize * paddingSize + y_ * paddingSize + x_;
	  cube->grid[index] = value;
	  }

	  if (border_x && border_z) {
	  index = z_ * paddingSize * paddingSize + y * paddingSize + x_;
	  cube->grid[index] = value;
	  }

	  if (border_y && border_z) {
	  index = z_ * paddingSize * paddingSize + y_ * paddingSize + x;
	  cube->grid[index] = value;
	  }

	  if (border_x && border_y && border_z) {
	  index = z_ * paddingSize * paddingSize + y_ * paddingSize + x_;
	  cube->grid[index] = value;
	  }

	}
      }
    }
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
