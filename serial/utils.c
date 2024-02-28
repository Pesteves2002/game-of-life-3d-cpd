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
  return cube;
}
