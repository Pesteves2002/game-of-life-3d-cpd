#include "utils.h"
#include <stdbool.h>

unsigned int seed;

void init_r4uni(int input_seed) { seed = input_seed + 987654321; }

float r4_uni() {
  int seed_in = seed;

  seed ^= (seed << 13);
  seed ^= (seed >> 17);
  seed ^= (seed << 5);

  return 0.5 + 0.2328306e-09 * (seed_in + (int)seed);
}

unsigned char *gen_initial_grid(long long N, float density, int input_seed,
                                int me, int nprocs, int dims[3],
                                MPI_Comm comm) {

  int chunk = N * N * N / nprocs;

  unsigned char *grid = (unsigned char *)calloc(chunk, sizeof(unsigned char));
  if (grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);

  int x_size = N / dims[0];
  int y_size = N / dims[1];
  int z_size = N / dims[2];

  int pos[3];

  if (MPI_Cart_coords(comm, me, 3, pos) != MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_coords error\n");
    return NULL;
  }

  int x_min = pos[0] * x_size;
  int x_max = (pos[0] + 1) * x_size;
  int y_min = pos[1] * y_size;
  int y_max = (pos[1] + 1) * y_size;
  int z_min = pos[2] * z_size;
  int z_max = (pos[2] + 1) * z_size;

  for (int z = 0; z < N; z++) {
    for (int y = 0; y < N; y++) {
      for (int x = 0; x < N; x++) {
        unsigned char value =
            r4_uni() < density ? (int)(r4_uni() * N_SPECIES) + 1 : 0;
        if (x >= x_min && x < x_max && y >= y_min && y < y_max && z >= z_min &&
            z < z_max) {
          grid[(z - z_min) * x_size * y_size + (y - y_min) * x_size +
               (x - x_min)] = value;
        }
      }
    }
  }

  return grid;
}
