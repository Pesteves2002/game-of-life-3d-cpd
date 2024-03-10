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
  if (cube->grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);

  for (int z = 0; z < paddingSize; z++) {
    for (int y = 0; y < paddingSize; y++) {
      for (int x = 0; x < paddingSize; x++) {
        if (x == 0 || y == 0 || z == 0 || x == paddingSize - 1 ||
            y == paddingSize - 1 || z == paddingSize - 1) {
          continue;
        } else {
          unsigned char value =
              r4_uni() < density ? (int)(r4_uni() * N_SPECIES) + 1 : 0;
          int index = z * paddingSize * paddingSize + y * paddingSize + x;
          cube->grid[index] = value;
          writeBorders(cube->grid, paddingSize, x, y, z, value);
        }
      }
    }
  }
  return cube;
}

void writeBorders(unsigned char *grid, int paddingSize, int x, int y, int z,
                  unsigned char value) {
  bool border_x = x == 1 || x == paddingSize - 2;
  bool border_y = y == 1 || y == paddingSize - 2;
  bool border_z = z == 1 || z == paddingSize - 2;

  int x_ = x != 1 ? 0 : (paddingSize - 1);
  int y_ = y != 1 ? 0 : (paddingSize - 1) * paddingSize;
  int z_ = z != 1 ? 0 : (paddingSize - 1) * paddingSize * paddingSize;

  y *= paddingSize;
  z *= paddingSize * paddingSize;

  if (border_x) {
    grid[z + y + x_] = value;
    if (border_y) {
      grid[z + y_ + x] = value;
      grid[z + y_ + x_] = value;
      if (border_z) {
        grid[z_ + y + x] = value;
        grid[z_ + y + x_] = value;
        grid[z_ + y_ + x] = value;
        grid[z_ + y_ + x_] = value;
      }
      return;
    }
    if (border_z) {
      grid[z_ + y + x] = value;
      grid[z_ + y + x_] = value;
    }
    return;
  }

  if (border_y) {
    grid[z + y_ + x] = value;
    if (border_z) {
      grid[z_ + y + x] = value;
      grid[z_ + y_ + x] = value;
    }
    return;
  }

  if (border_z) {
    grid[z_ + y + x] = value;
  }
};
