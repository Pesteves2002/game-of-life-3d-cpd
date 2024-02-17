#include <omp.h>
#include <cstdio>

int main () {
  double exec_time;

  // grid = gen_initial_grid(N, seed, density);
  exec_time = -omp_get_wtime();

  // simulation();

  exec_time += omp_get_wtime();
  fprintf(stderr, "%.1fs\n", exec_time);

  return 0;
}

