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
                                int me, int nprocs, int dims[1],
                                MPI_Comm comm) {

  int guaranteed_chunk = N / nprocs; // lower bound

  int remainder = N % nprocs; // remainder

  int chunk_size =
      (remainder >= nprocs - me) ? guaranteed_chunk + 1 : guaranteed_chunk;

  // allocs either guaranteed_chunk or guaranteed_chunk + 1
  unsigned char *grid =
      (unsigned char *)calloc(chunk_size * N * N, sizeof(unsigned char));
  if (grid == NULL) {
    printf("Failed to allocate matrix\n");
    exit(1);
  }

  init_r4uni(input_seed);

  int pos[1];
  if (MPI_Cart_coords(comm, me, 1, pos) != MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_coords error\n");
    return NULL;
  }

  int z_min;
  if (remainder >= nprocs - me) {
    z_min = (nprocs - remainder) * (guaranteed_chunk) +
            (me - (nprocs - remainder)) * (guaranteed_chunk + 1);
  } else {
    z_min = pos[0] * guaranteed_chunk;
  }

  int z_max = z_min + chunk_size;


  for (int z = 0; z < N; z++) {
    for (int y = 0; y < N; y++) {
      for (int x = 0; x < N; x++) {
        unsigned char value =
            r4_uni() < density ? (int)(r4_uni() * N_SPECIES) + 1 : 0;
        if (z >= z_min && z < z_max) {
          int index = (z - z_min) * N * N + y * N + x;
          grid[index] = value;
        }
      }
    }
  }

  return grid;
}
