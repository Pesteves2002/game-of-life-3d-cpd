#include "utils.h"

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
  cube->grid = (Cell *)malloc(N * N * N * sizeof(Cell));
  if (cube->grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);
  for (unsigned long long x = 0; x < N * N * N; x++)
    cube->grid[x].rightState =
        r4_uni() < density ? (int)(r4_uni() * N_SPECIES) + 1 : 0;

  return cube;
}
