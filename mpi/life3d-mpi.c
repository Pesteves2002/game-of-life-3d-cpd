#include "simulation.h"
#include <omp.h>

int main(int argc, char *argv[]) {

  if (argc != 5) {
    fprintf(
        stderr,
        "Usage: life3d <num_generations> <cells_per_side> <density> <seed>\n");
    return 1;
  }

  if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
    fprintf(stderr, "MPI initialization error\n");
    return 1;
  }

  int me, nprocs;
  if (MPI_Comm_size(MPI_COMM_WORLD, &nprocs) != MPI_SUCCESS) {
    fprintf(stderr, "MPI rank error\n");
    return 1;
  }

  const int ndims = 3;
  int dims[] = {0, 0, 0};
  if (MPI_Dims_create(nprocs, ndims, dims) != MPI_SUCCESS) {
    fprintf(stderr, "MPI Dims_create error\n");
    return 1;
  }
  const int periods[] = {true, true, true};
  const int reorder = true;

  MPI_Comm comm_cart;

  if (MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder,
                      &comm_cart) != MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_create error\n");
    return 1;
  }
  if (MPI_Comm_rank(comm_cart, &me) != MPI_SUCCESS) {
    fprintf(stderr, "MPI rank error\n");
    return 1;
  }

  int gen_num = atoi(argv[1]);
  long long grid_size = atoi(argv[2]);
  float density = atof(argv[3]);
  int input_seed = atoi(argv[4]);

  unsigned char *grid = gen_initial_grid(grid_size, density, input_seed, me,
                                         nprocs, dims, comm_cart);
  initializeAux(grid, gen_num, grid_size, me, nprocs, dims, comm_cart);

  double exec_time;
  exec_time = -omp_get_wtime();

  simulation();

  exec_time += omp_get_wtime();

  // only the master process prints the execution time
  if (me == 0) {
    fprintf(stderr, "%.1fs\n", exec_time);
    printLeaderboard();
  }

  MPI_Finalize();
  return 0;
}
