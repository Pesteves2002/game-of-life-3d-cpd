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
  cube->grid = (Cell *)calloc(N * N * N, sizeof(Cell));
  if (cube->grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);
  for (unsigned long long x = 0; x < N * N * N; x++) {
    unsigned char state =
        r4_uni() < density ? (int)(r4_uni() * N_SPECIES) + 1 : 0;
    cube->grid[x].rightState = state;
    cube->grid[x].lastModifiedEven = true;
  }

  for (int z = 0; z < N; z++) {
    for (int y = 0; y < N; y++) {
      for (int x = 0; x < N; x++) {
        updateNeighborsCount(cube, x, y, z, true,
                             GET_CELL(cube, x, y, z).rightState == 0 ? 0 : 1);
      }
    }
  }

  return cube;
}

void updateNeighborsCount(Cube *cube, int x, int y, int z, bool even_gen,
                          unsigned char value) {
  for (int k = -1; k <= 1; k++) {
    int z_ = (z + k + cube->side_size) % cube->side_size;
    for (int j = -1; j <= 1; j++) {
      int y_ = (y + j + cube->side_size) % cube->side_size;
      for (int i = -1; i <= 1; i++) {
        int x_ = (x + i + cube->side_size) % cube->side_size;
        if (i == 0 && j == 0 && k == 0) {
          continue;
        }

        Cell *cell = &GET_CELL(cube, x_, y_, z_);

        if (cell->lastModifiedEven != even_gen) {
          cell->lastModifiedEven = even_gen;
          if (even_gen) {
            cell->rightNeighbourCount = cell->leftNeighbourCount;
          } else {
            cell->leftNeighbourCount = cell->rightNeighbourCount;
          }
        }

        if (even_gen) {
          GET_CELL(cube, x_, y_, z_).rightNeighbourCount += value;
        } else {
          GET_CELL(cube, x_, y_, z_).leftNeighbourCount += value;
        }
      }
    }
  }
};
