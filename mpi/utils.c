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

unsigned char *gen_initial_grid(long long N, float density, int input_seed,
                                int me, int nprocs) {

  int total = N * N * N;
  int chunk = total / nprocs;
  int start = me * chunk;
  int end = (me + 1) * chunk;

  unsigned char *grid = (unsigned char *)calloc(chunk, sizeof(unsigned char));
  if (grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);

  int counter = 0;

  for (int z = 0; z < N; z++) {
    for (int y = 0; y < N; y++) {
      for (int x = 0; x < N; x++) {
        unsigned char value =
            r4_uni() < density ? (int)(r4_uni() * N_SPECIES) + 1 : 0;
        if (counter >= start && counter < end) {
          grid[counter - start] = value;
        }
        counter++;
      }
    }
  }

  return grid;
}

void writeBorders(unsigned char *grid, int paddingSize, int x, int y, int z,
                  unsigned char value) {
  bool border_x = x == 1 || x == paddingSize - 2;
  bool border_y = y == 1 || y == paddingSize - 2;
  bool border_z = z == 1 || z == paddingSize - 2;
  if (!border_x && !border_y && !border_z) {
    return;
  }

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
