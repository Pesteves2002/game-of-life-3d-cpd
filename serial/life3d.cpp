#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <omp.h>

int gen_num;
long long cells_per_side;
float density;
int input_seed;

char ***grid;

int main(int argc, char *argv[]) {

  if (argc != 5) {
    fprintf(
        stderr,
        "Usage: life3d <num_generations> <cells_per_side> <density> <seed>\n");
    return 1;
  }

  gen_num = atoi(argv[1]);
  cells_per_side = atoi(argv[2]);
  density = atof(argv[3]);
  input_seed = atoi(argv[4]);

  double exec_time;
  grid = gen_initial_grid(cells_per_side, density, input_seed);
  exec_time = -omp_get_wtime();

  // simulation();

  exec_time += omp_get_wtime();
  fprintf(stderr, "%.1fs\n", exec_time);

  return 0;
}
