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
  cube->neighbourCount = (unsigned char *)calloc(
      paddingSize * paddingSize * paddingSize, sizeof(unsigned char));
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
        }
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

        updateNeighborsCount(cube->neighbourCount, paddingSize, x, y, z,
                             value == 0 ? 0 : 1);
      }
    }
  }

  return cube;
}

void updateNeighborsCount(unsigned char *cache, long long paddingSize, int x,
                          int y, int z, unsigned char value) {

  for (int k = -1; k <= 1; k++) {
    int z_ = z + k;
    if (z_ == 0) {
      z_ = paddingSize - 2;
    }
    if (z_ == paddingSize - 1) {
      z_ = 1;
    }
    z_ *= paddingSize * paddingSize;
    for (int j = -1; j <= 1; j++) {
      int y_ = y + j;
      if (y_ == 0) {
        y_ = paddingSize - 2;
      }
      if (y_ == paddingSize - 1) {
        y_ = 1;
      }
      y_ *= paddingSize;
      for (int i = -1; i <= 1; i++) {
        if (i == 0 && j == 0 && k == 0) {
          continue;
        }

        int x_ = (x + i);
        if (x_ == 0) {
          x_ = paddingSize - 2;
        }
        if (x_ == paddingSize - 1) {
          x_ = 1;
        }

        int index = z_ + y_ + x_;

        cache[index] += value;
      }
    }
  }
}

void debugPrintGrid(Cube *cube, int gridPadding) {
  for (int z = 0; z < gridPadding; z++) {
    for (int y = 0; y < gridPadding; y++) {
      for (int x = 0; x < gridPadding; x++) {
        int index = z * gridPadding * gridPadding + y * gridPadding + x;
        int valueToPrint = (int)cube->grid[index];
        if (valueToPrint == 0) {
          fprintf(stdout, "  ");
        } else {
          fprintf(stdout, "%d ", valueToPrint);
        }
      }

      fprintf(stdout, "\n");
    }

    fprintf(stdout, "||||\n");
  }

  fprintf(stdout, "---\n");
};

void debugPrintNeighbourCount(Cube *cube, int gridPadding) {
  for (int z = 0; z < gridPadding; z++) {
    for (int y = 0; y < gridPadding; y++) {
      for (int x = 0; x < gridPadding; x++) {
        int index = z * gridPadding * gridPadding + y * gridPadding + x;
        fprintf(stdout, "%d ", (int)cube->neighbourCount[index]);
      }
      fprintf(stdout, "\n");
    }

    fprintf(stdout, "||||\n");
  }

  fprintf(stdout, "---\n");
};
