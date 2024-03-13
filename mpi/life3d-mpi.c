#include "simulation.h"
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {

  if (argc != 5) {
    fprintf(
        stderr,
        "Usage: life3d <num_generations> <cells_per_side> <density> <seed>\n");
    return 1;
  }

  MPI_Init(&argc, &argv);

  int gen_num = atoi(argv[1]);
  long long grid_size = atoi(argv[2]);
  float density = atof(argv[3]);
  int input_seed = atoi(argv[4]);

  int me, nprocs;

  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  double exec_time;
  unsigned char *grid =
      gen_initial_grid(grid_size, density, input_seed, me, nprocs);
  initializeAux(grid, gen_num, grid_size, me, nprocs);

  exec_time = -omp_get_wtime();

  // simulation();

  exec_time += omp_get_wtime();

  // only the master process prints the execution time
  if (me == 0) {
    fprintf(stderr, "%.1fs\n", exec_time);
    printLeaderboard();
  }

  MPI_Finalize();
  return 0;
}
