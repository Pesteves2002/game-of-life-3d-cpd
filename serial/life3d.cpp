#include "simulation.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <omp.h>

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
  char ***g = gen_initial_grid(grid_size, density, input_seed);
  exec_time = -omp_get_wtime();

  simulation(&g, gen_num, grid_size);

  exec_time += omp_get_wtime();
  fprintf(stderr, "%.1fs\n", exec_time);

  printLeaderboard();
  return 0;
}
