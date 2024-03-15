#include "simulation.h"
#include <omp.h>

int main(int argc, char *argv[]) {

  if (argc != 5) {
    fprintf(
        stderr,
        "Usage: life3d <num_generations> <cells_per_side> <density> <seed>\n");
    return 1;
  }

  int gen_num = atoi(argv[1]);
  long long grid_size = atoll(argv[2]);
  float density = atof(argv[3]);
  int input_seed = atoi(argv[4]);

  double exec_time;
  unsigned char *grid = gen_initial_grid(grid_size, density, input_seed);
  initializeAux(grid, gen_num, grid_size);

  exec_time = -omp_get_wtime();

  simulation();

  exec_time += omp_get_wtime();
  fprintf(stderr, "%.1f\n", exec_time);

  printLeaderboard();
  return 0;
}
