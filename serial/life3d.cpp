#include "simulation.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <omp.h>

char ***grid;                            // defined in simulation.h
int gen_num;                             // defined in simulation.h
long long grid_size;                     // defined in simulation.h
long long leaderboard[LEADERBOARD_SIZE]; // defined in simulation.h

int main(int argc, char *argv[]) {

  if (argc != 5) {
    fprintf(
        stderr,
        "Usage: life3d <num_generations> <cells_per_side> <density> <seed>\n");
    return 1;
  }

  float density;
  int input_seed;

  gen_num = atoi(argv[1]);
  grid_size = atoi(argv[2]);
  density = atof(argv[3]);
  input_seed = atoi(argv[4]);

  double exec_time;
  grid = gen_initial_grid(grid_size, density, input_seed);
  exec_time = -omp_get_wtime();

  simulation();

  exec_time += omp_get_wtime();
  fprintf(stderr, "%.1fs\n", exec_time);

  return 0;
}
