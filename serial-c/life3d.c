#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N_SPECIES 9

typedef struct {
  unsigned char leftState : 4;
  unsigned char rightState : 4;
} Cell;

typedef struct {
	unsigned long long side_size;
  Cell* grid;
} Cube;

#define GET_CELL(cube, x, y, z) cube->grid[x + cube->side_size * (y + cube->side_size * z)]

#define SET_CELL_LEFT_STATE(cube, x, y, z, value) \
	GET_CELL(cube, x, y, z).leftState = value;

#define SET_CELL_RIGHT_STATE(cube, x, y, z, value) \
	GET_CELL(cube, x, y, z).rightState = value;

int seed;

void init_r4uni(int input_seed) { seed = input_seed + 987654321; }

float r4_uni() {
  int seed_in = seed;

  seed ^= (seed << 13);
  seed ^= (seed >> 17);
  seed ^= (seed << 5);

  return 0.5 + 0.2328306e-09 * (seed_in + (int)seed);
}

Cube* gen_initial_cube(long long N, float density, int input_seed) {
  Cube *cube = (Cube *) malloc(sizeof(Cube));
  if (cube == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

	cube->side_size = N;
  cube->grid = (Cell *) malloc(N * N * N * sizeof(Cell));
  if (cube->grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);
  for (unsigned long long x = 0; x < N * N * N; x++)
        if (r4_uni() < density)
          cube->grid[x].rightState = (int)(r4_uni() * N_SPECIES) + 1;

	return cube;
}

int main(int argc, char *argv[]) {

  if (argc != 5) {
    fprintf(
        stderr,
        "Usage: life3d <num_generations> <cells_per_side> <density> <seed>\n");
    return 1;
  }

  int gen_num = atoi(argv[1]);
  int grid_size = atoi(argv[2]);
  float density = atof(argv[3]);
  int input_seed = atoi(argv[4]);

  double exec_time;
  Cube *cube = gen_initial_cube(grid_size, density, input_seed);
  exec_time = -omp_get_wtime();

  // simulation(&g, gen_num, grid_size);

  exec_time += omp_get_wtime();
  fprintf(stderr, "%.1fs\n", exec_time);

  // printLeaderboard();
  return 0;
}
