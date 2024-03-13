#include "simulation.h"
#include <omp.h>

int main(int argc, char *argv[]) {

  if (argc != 5) {
    fprintf(
        stderr,
        "Usage: life3d <num_generations> <cells_per_side> <density> <seed>\n");
    return 1;
  }

  MPI_Init(&argc, &argv);

  int me, nprocs;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  const int ndims = 3;
  int dims[] = {0, 0, 0};
  MPI_Dims_create(nprocs, ndims, dims);
  const int periods[] = {1, 1, 1};
  const int reorder = 1;

  MPI_Comm comm_cart;

  MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &comm_cart);
  MPI_Comm_rank(comm_cart, &me);

  int gen_num = atoi(argv[1]);
  long long grid_size = atoi(argv[2]);
  float density = atof(argv[3]);
  int input_seed = atoi(argv[4]);

  unsigned char *grid =
      gen_initial_grid(grid_size, density, input_seed, me, nprocs);
  initializeAux(grid, gen_num, grid_size, me, nprocs, comm_cart);

  double exec_time;
  exec_time = -omp_get_wtime();

  simulation();

  exec_time += omp_get_wtime();

  // only the master process prints the execution time
  if (me == 0) {
    // fprintf(stderr, "%.1fs\n", exec_time);
    // printLeaderboard();
  }

  MPI_Finalize();
  return 0;
}
