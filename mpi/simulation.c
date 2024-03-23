#include "simulation.h"
#include <assert.h>

unsigned char *grid;
int grid_size;
int genNum;

long long leaderboard[(N_SPECIES + 1) * 3] = {0}; // current, max, max gen

MPI_Comm comm_cart;
int coords[3];
int me;
int nprocs;
int num_blocks;

MPI_Request requests[4];
MPI_Status statuses[4];

int z_size, y_size, x_size; // num of blocks in each dimension

int area_zy;
int area_xz;
int area_xy;

unsigned char *payload_neg_x;
unsigned char *payload_pos_x;

unsigned char *payload_neg_y;
unsigned char *payload_pos_y;

unsigned char *payload_neg_z;
unsigned char *payload_pos_z;

unsigned char *neg_x;
unsigned char *pos_x;

unsigned char *neg_y;
unsigned char *pos_y;

unsigned char *neg_z;
unsigned char *pos_z;

int aux_x_size;
int aux_y_size;
int aux_z_size;

unsigned char *aux_x;
unsigned char *aux_y;
unsigned char *aux_z;

int chunk_size;

void initializeAux(unsigned char *g, int num, int size, int m, int procs,
                   int axis[1], MPI_Comm comm) {
  grid = g;
  grid_size = size;
  genNum = num;
  me = m;
  nprocs = procs;
  comm_cart = comm;

  int guaranteed_chunk = size / nprocs; // lower bound

  int remainder = size % nprocs; // remainder

  chunk_size =
      (remainder >= nprocs - me) ? guaranteed_chunk + 1 : guaranteed_chunk;

  long long paddingSize = size + 2;
  x_size = paddingSize;
  y_size = paddingSize;
  area_xy = paddingSize * paddingSize;

  if (MPI_Cart_coords(comm_cart, me, 3, coords) != MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_coords error\n");
    return;
  }

  payload_neg_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));
  payload_pos_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));

  neg_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));
  pos_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));

  aux_z_size = paddingSize * paddingSize * (chunk_size + 2);
  aux_z = (unsigned char *)malloc(aux_z_size * sizeof(unsigned char));
};

void printDebugZ() {
  printf("aux_z\n");
  printf("x_size: %d, y_size: %d, z_size: %d\n", x_size, y_size, z_size);
  printf("expected size: %d %d %d\n", x_size + 2, y_size + 2, z_size + 2);
  for (int i = 0; i < aux_z_size; i++) {
    if (i % (2 + x_size) == 0 && i != 0) {
      printf("\n");
    }

    if (i % ((2 + x_size) * (2 + y_size)) == 0) {
      printf("||||\n");
    }
    if (aux_z[i] == 0) {
      printf(" ");
    } else {
      printf("%d ", aux_z[i]);
    }
  }
  printf("\n");
};

void exchangeZ() {
  int neg_z_rank, pos_z_rank;

  if (MPI_Cart_shift(comm_cart, 0, 1, &neg_z_rank, &pos_z_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  // Copy the first n nums of the local grid to the payload (layer 0 and layer
  // n-1)
  memcpy(payload_neg_z, grid, area_xy * sizeof(unsigned char));

  int last_layer_index =
      (chunk_size * area_xy) - area_xy; // index of the last layer
  memcpy(payload_pos_z, grid + last_layer_index,
         area_xy * sizeof(unsigned char));


  MPI_Irecv(neg_z, area_xy, MPI_UNSIGNED_CHAR, neg_z_rank, 2, comm_cart,
            &requests[0]);
  MPI_Irecv(pos_z, area_xy, MPI_UNSIGNED_CHAR, pos_z_rank, 1, comm_cart,
            &requests[1]);

  MPI_Isend(payload_neg_z, area_xy, MPI_UNSIGNED_CHAR, neg_z_rank, 1, comm_cart,
            &requests[2]);
  MPI_Isend(payload_pos_z, area_xy, MPI_UNSIGNED_CHAR, pos_z_rank, 2, comm_cart,
            &requests[3]);

  MPI_Waitall(4, requests, statuses);

  memset(aux_z, 10, aux_z_size * sizeof(unsigned char));

  memcpy(aux_z, neg_z, area_xy * sizeof(unsigned char));

  memcpy(aux_z + (aux_z_size - area_xy), pos_z,
         area_xy * sizeof(unsigned char));

  int index = 0;
  for (int i = 0; i < chunk_size * (grid_size + 2) * (grid_size + 2); i++) {
    while (aux_z[index] != 10) {
      index++;
    }
    aux_z[index] = grid[i];
  }

  // if (me == 0) {
  //   fprintf(stdout, "aux_z\n");
  //   for (int i = 0; i < aux_z_size; i++) {
  //     if (i % (grid_size + 2) == 0 && i != 0) {
  //       fprintf(stdout, "\n");
  //     }
  //     if (i % (area_xy) == 0) {
  //       fprintf(stdout, "||||\n");
  //     }
  //
  //     fprintf(stdout, "%d ", aux_z[i]);
  //   }
  //   fprintf(stdout, "\n\n");
  // }
  if (me == 0) {
    // printDebugZ();
  }
}

void receiveLeaderboards() {
  if (me == 0) {
    MPI_Reduce(MPI_IN_PLACE, leaderboard, (N_SPECIES + 1), MPI_LONG_LONG,
               MPI_SUM, 0, comm_cart);
  } else {
    MPI_Reduce(leaderboard, MPI_IN_PLACE, (N_SPECIES + 1), MPI_LONG_LONG,
               MPI_SUM, 0, comm_cart);
    memset(leaderboard, 0, (N_SPECIES + 1) * sizeof(long long));
  }
}

void exchangeMessages() {
  // exchangeX();
  // exchangeY();
  exchangeZ();
}

void simulation() {
  // Initialize leaderboard with the initial state
#pragma omp parallel for reduction(+ : leaderboard[ : N_SPECIES + 1])
  for (int z = 0; z < chunk_size; z++) {
    for (int y = 1; y < grid_size + 1; y++) {
      for (int x = 1; x < grid_size + 1; x++) {
        int index =
            z * (grid_size + 2) * (grid_size + 2) + y * (grid_size + 2) + x;
        leaderboard[grid[index]]++;
      }
    }
  }

  receiveLeaderboards();
  if (me == 0) {
    updateMaxScores(0);
  }

  MPI_Barrier(comm_cart);

  // generations start at 1
  for (int gen = 1; gen < genNum + 1; gen++) {
    exchangeMessages();

    // iterate over the aux_z
#pragma omp parallel for reduction(+ : leaderboard[ : N_SPECIES + 1])
    for (int z = 1; z < chunk_size +  1 ; z++) {
      for (int y = 1; y < grid_size + 1; y++) {
        for (int x = 1; x < grid_size + 1; x++) {
          int index = (z) * (grid_size + 2) * (grid_size + 2) +
		      (y ) * (grid_size + 2) + x;
          leaderboard[updateCellState(x, y, z, index)]++;
        }
      }
    }

    receiveLeaderboards();

    if (me == 0) {
      updateMaxScores(gen);
    }

    MPI_Barrier(comm_cart);
  }
};

unsigned char updateCellState(int x, int y, int z, int index) {
  unsigned char current_state = aux_z[index];

  unsigned char new_state = calculateNextState(x, y, z, current_state, index);

  if (current_state != new_state) {
	  int w = (z - 1) * (grid_size + 2) * (grid_size + 2) + (y) * (grid_size + 2) + (x);
    grid[w] = new_state;
    writeBorders(grid, x_size,  x, y, z-1, new_state);
  }

  return new_state;
};

// wraps around the grid
unsigned char calculateNextState(int x, int y, int z,
                                 unsigned char current_state, int index) {

  unsigned char neighbourCount = getNeighbourCount(x, y, z);
  if (current_state == 0) {
    if (!(neighbourCount >= 7 && neighbourCount <= 10)) {
      return 0;
    }

    return getMostFrequentValue(x, y, z);
  }

  return (neighbourCount <= 4 || neighbourCount > 13) ? 0 : current_state;
};

unsigned char getNeighbourCount(int x, int y, int z) {
  unsigned char count = 0;

  int z_disp = x_size * y_size;
  int y_disp = x_size;

  int z1 = (z - 1) * z_disp;
  int z2 = z * z_disp;
  int z3 = (z + 1) * z_disp;
  int y1 = (y - 1) * y_disp;
  int y2 = y * y_disp;
  int y3 = (y + 1) * y_disp;
  int x1 = x - 1;
  int x2 = x;
  int x3 = x + 1;

  count += (aux_z[z1 + y1 + x1] != 0);
  count += (aux_z[z1 + y1 + x2] != 0);
  count += (aux_z[z1 + y1 + x3] != 0);
  count += (aux_z[z1 + y2 + x1] != 0);
  count += (aux_z[z1 + y2 + x2] != 0);
  count += (aux_z[z1 + y2 + x3] != 0);
  count += (aux_z[z1 + y3 + x1] != 0);
  count += (aux_z[z1 + y3 + x2] != 0);
  count += (aux_z[z1 + y3 + x3] != 0);

  count += (aux_z[z2 + y1 + x1] != 0);
  count += (aux_z[z2 + y1 + x2] != 0);
  count += (aux_z[z2 + y1 + x3] != 0);
  count += (aux_z[z2 + y2 + x1] != 0);
  count += (aux_z[z2 + y2 + x3] != 0);
  count += (aux_z[z2 + y3 + x1] != 0);
  count += (aux_z[z2 + y3 + x2] != 0);
  count += (aux_z[z2 + y3 + x3] != 0);

  count += (aux_z[z3 + y1 + x1] != 0);
  count += (aux_z[z3 + y1 + x2] != 0);
  count += (aux_z[z3 + y1 + x3] != 0);
  count += (aux_z[z3 + y2 + x1] != 0);
  count += (aux_z[z3 + y2 + x2] != 0);
  count += (aux_z[z3 + y2 + x3] != 0);
  count += (aux_z[z3 + y3 + x1] != 0);
  count += (aux_z[z3 + y3 + x2] != 0);
  count += (aux_z[z3 + y3 + x3] != 0);

  return count;
};

unsigned char getMostFrequentValue(int x, int y, int z) {
  unsigned char neighborsValues[N_SPECIES + 1] = {0};
  int z_disp = x_size * y_size;
  int y_disp = x_size ;

  int z1 = (z - 1) * z_disp;
  int z2 = z * z_disp;
  int z3 = (z + 1) * z_disp;
  int y1 = (y - 1) * y_disp;
  int y2 = y * y_disp;
  int y3 = (y + 1) * y_disp;
  int x1 = x - 1;
  int x2 = x;
  int x3 = x + 1;

  neighborsValues[aux_z[z1 + y1 + x1]]++;
  neighborsValues[aux_z[z1 + y1 + x2]]++;
  neighborsValues[aux_z[z1 + y1 + x3]]++;
  neighborsValues[aux_z[z1 + y2 + x1]]++;
  neighborsValues[aux_z[z1 + y2 + x2]]++;
  neighborsValues[aux_z[z1 + y2 + x3]]++;
  neighborsValues[aux_z[z1 + y3 + x1]]++;
  neighborsValues[aux_z[z1 + y3 + x2]]++;
  neighborsValues[aux_z[z1 + y3 + x3]]++;

  neighborsValues[aux_z[z2 + y1 + x1]]++;
  neighborsValues[aux_z[z2 + y1 + x2]]++;
  neighborsValues[aux_z[z2 + y1 + x3]]++;
  neighborsValues[aux_z[z2 + y2 + x1]]++;
  neighborsValues[aux_z[z2 + y2 + x3]]++;
  neighborsValues[aux_z[z2 + y3 + x1]]++;
  neighborsValues[aux_z[z2 + y3 + x2]]++;
  neighborsValues[aux_z[z2 + y3 + x3]]++;

  neighborsValues[aux_z[z3 + y1 + x1]]++;
  neighborsValues[aux_z[z3 + y1 + x2]]++;
  neighborsValues[aux_z[z3 + y1 + x3]]++;
  neighborsValues[aux_z[z3 + y2 + x1]]++;
  neighborsValues[aux_z[z3 + y2 + x2]]++;
  neighborsValues[aux_z[z3 + y2 + x3]]++;
  neighborsValues[aux_z[z3 + y3 + x1]]++;
  neighborsValues[aux_z[z3 + y3 + x2]]++;
  neighborsValues[aux_z[z3 + y3 + x3]]++;

  unsigned char mostFrequentValue = 0;
  int maxCount = 0;
  for (int i = 1; i < N_SPECIES + 1; i++) {
    if (neighborsValues[i] > maxCount) {
      maxCount = neighborsValues[i];
      mostFrequentValue = i;
    }
  }
  return mostFrequentValue;
};

void debugPrintGrid() {
  for (int z = 0; z < z_size; z++) {
    for (int y = 0; y < y_size; y++) {
      for (int x = 0; x < x_size; x++) {
        int index = z * y_size * x_size + y * x_size + x;
        int valueToPrint = (int)grid[index];
        if (valueToPrint == 0) {
          fprintf(stdout, " ");
        } else {
          fprintf(stdout, "%d ", valueToPrint);
        }
      }

      fprintf(stdout, "\n");
    }

    fprintf(stdout, "||||\n");
  }

  fprintf(stdout, "---\n");
};

void updateMaxScores(int current_gen) {
  for (int i = 1; i < N_SPECIES + 1; i++) {
    if (leaderboard[i] > leaderboard[i + N_SPECIES]) {
      leaderboard[i + N_SPECIES] = leaderboard[i];
      leaderboard[i + N_SPECIES * 2] = current_gen;
    }
    leaderboard[i] = 0;
  }
};

void printLeaderboard() {
  for (int i = 1; i < N_SPECIES + 1; i++) {
    fprintf(stdout, "%d %lld %lld\n", i, leaderboard[i + N_SPECIES],
            leaderboard[i + N_SPECIES * 2]);
  }
};
