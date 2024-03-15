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

void initializeAux(unsigned char *g, int num, int size, int m, int procs,
                   int axis[3], MPI_Comm comm) {
  grid = g;
  grid_size = size;
  genNum = num;
  me = m;
  nprocs = procs;
  comm_cart = comm;
  x_size = grid_size / axis[0];
  y_size = grid_size / axis[1];
  z_size = grid_size / axis[2];

  if (MPI_Cart_coords(comm_cart, me, 3, coords) != MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_coords error\n");
    return;
  }

  num_blocks = x_size * y_size * z_size;

  area_zy = z_size * y_size;
  area_xz = (x_size + 2) * z_size;
  area_xy = (x_size + 2) * (y_size + 2);

  payload_neg_x = (unsigned char *)malloc(area_zy * sizeof(unsigned char));
  payload_pos_x = (unsigned char *)malloc(area_zy * sizeof(unsigned char));

  payload_neg_y = (unsigned char *)malloc(area_xz * sizeof(unsigned char));
  payload_pos_y = (unsigned char *)malloc(area_xz * sizeof(unsigned char));

  payload_neg_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));
  payload_pos_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));

  neg_x = (unsigned char *)malloc(area_zy * sizeof(unsigned char));
  pos_x = (unsigned char *)malloc(area_zy * sizeof(unsigned char));

  neg_y = (unsigned char *)malloc(area_xz * sizeof(unsigned char));
  pos_y = (unsigned char *)malloc(area_xz * sizeof(unsigned char));

  neg_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));
  pos_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));

  aux_x_size = (2 + x_size) * y_size * z_size;
  aux_y_size = (2 + x_size) * (2 + y_size) * z_size;
  aux_z_size = (2 + x_size) * (2 + y_size) * (2 + z_size);

  aux_x = (unsigned char *)malloc(aux_x_size * sizeof(unsigned char));
  aux_y = (unsigned char *)malloc(aux_y_size * sizeof(unsigned char));
  aux_z = (unsigned char *)malloc(aux_z_size * sizeof(unsigned char));
};

void printDebugX() {
  printf("aux_x\n");
  printf("x_size: %d, y_size: %d, z_size: %d\n", x_size, y_size, z_size);
  printf("expected size: %d %d %d\n", x_size + 2, y_size, z_size);
  for (int i = 0; i < aux_x_size; i++) {
    if (i % (2 + x_size) == 0 && i != 0) {
      printf("\n");
    }

    if (i % ((2 + x_size) * y_size) == 0) {
      printf("\n");
    }
    if (aux_x[i] == 0) {
      printf("0");
    } else {
      printf("%d", aux_x[i]);
    }
  }
  printf("\n");
};

void printDebugY() {
  printf("aux_y\n");
  printf("x_size: %d, y_size: %d, z_size: %d\n", x_size, y_size, z_size);
  printf("expected size: %d %d %d\n", x_size + 2, y_size + 2, z_size);
  for (int i = 0; i < aux_y_size; i++) {
    if (i % (2 + x_size) == 0 && i != 0) {
      printf("\n");
    }

    if (i % ((2 + x_size) * (y_size + 2)) == 0) {
      printf("\n");
    }
    if (aux_y[i] == 0) {
      printf("0");
    } else {
      printf("%d", aux_y[i]);
    }
  }
  printf("\n");
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

void exchangeX() {
  int neg_x_rank, pos_x_rank;

  if (MPI_Cart_shift(comm_cart, 0, 1, &neg_x_rank, &pos_x_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  int count_pos = 0;
  int count_neg = 0;
  for (int i = 0; i < num_blocks; i++) {
    if (i % x_size == 0) {
      payload_neg_x[count_neg] = grid[i];
      count_neg++;
    }
    if (i % x_size == x_size - 1) {
      payload_pos_x[count_pos] = grid[i];
      count_pos++;
    }
  }

  MPI_Irecv(neg_x, area_zy, MPI_UNSIGNED_CHAR, neg_x_rank, 2, comm_cart,
            &requests[0]);
  MPI_Irecv(pos_x, area_zy, MPI_UNSIGNED_CHAR, pos_x_rank, 1, comm_cart,
            &requests[1]);

  MPI_Isend(payload_neg_x, area_zy, MPI_UNSIGNED_CHAR, neg_x_rank, 1, comm_cart,
            &requests[2]);
  MPI_Isend(payload_pos_x, area_zy, MPI_UNSIGNED_CHAR, pos_x_rank, 2, comm_cart,
            &requests[3]);

  MPI_Waitall(4, requests, statuses);

  memset(aux_x, 10, aux_x_size * sizeof(unsigned char));
  for (int i = 0; i < area_zy; i++) {
    aux_x[i * (2 + x_size)] = neg_x[i];
    aux_x[i * (2 + x_size) + x_size + 1] = pos_x[i];
  }

  // Fill the rest of the aux_x array with the local grid
  int index = 0;
  for (int i = 0; i < num_blocks; i++) {
    while (aux_x[index] != 10) {
      index++;
    }
    aux_x[index] = grid[i];
  }
  if (me == 0) {
    // printDebugX();
  }
}

void exchangeY() {
  int neg_y_rank, pos_y_rank;

  if (MPI_Cart_shift(comm_cart, 1, 1, &neg_y_rank, &pos_y_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  int count_neg = 0;
  int count_pos = 0;
  for (int z = 0; z < z_size; z++) {
    for (int x = 0; x < (x_size + 2); x++) {
      int index = z * (x_size + 2) * y_size + x;
      payload_neg_y[count_neg] = aux_x[index];
      count_neg++;
      index = (z + 1) * (x_size + 2) * y_size - (x_size + 2) + x;
      payload_pos_y[count_pos] = aux_x[index];
      count_pos++;
    }
  }

  MPI_Irecv(neg_y, area_xz, MPI_UNSIGNED_CHAR, neg_y_rank, 2, comm_cart,
            &requests[0]);
  MPI_Irecv(pos_y, area_xz, MPI_UNSIGNED_CHAR, pos_y_rank, 1, comm_cart,
            &requests[1]);

  MPI_Isend(payload_neg_y, area_xz, MPI_UNSIGNED_CHAR, neg_y_rank, 1, comm_cart,
            &requests[2]);
  MPI_Isend(payload_pos_y, area_xz, MPI_UNSIGNED_CHAR, pos_y_rank, 2, comm_cart,
            &requests[3]);

  MPI_Waitall(4, requests, statuses);

  memset(aux_y, 10, aux_y_size * sizeof(unsigned char));
  for (int z = 0; z < z_size; z++) {
    for (int x = 0; x < (x_size + 2); x++) {
      int index = z * (x_size + 2) * (y_size + 2) + x;
      aux_y[index] = neg_y[z * (x_size + 2) + x];
      index = (z + 1) * (x_size + 2) * (y_size + 2) - (x_size + 2) + x;
      aux_y[index] = pos_y[z * (x_size + 2) + x];
    }
  }

  int index = 0;
  for (int i = 0; i < aux_x_size; i++) {
    while (aux_y[index] != 10) {
      index++;
    }
    aux_y[index] = aux_x[i];
  }

  if (me == 0) {
    // printDebugY();
  }
}

void exchangeZ() {
  int neg_z_rank, pos_z_rank;

  if (MPI_Cart_shift(comm_cart, 2, 1, &neg_z_rank, &pos_z_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  // Copy the first n nums of the local grid to the payload (layer 0 and layer
  // n-1)
  memcpy(payload_neg_z, aux_y, area_xy * sizeof(unsigned char));
  memcpy(payload_pos_z, aux_y + aux_y_size - area_xy,
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
  for (int i = 0; i < aux_y_size; i++) {
    while (aux_z[index] != 10) {
      index++;
    }
    aux_z[index] = aux_y[i];
  }
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
  exchangeX();
  exchangeY();
  exchangeZ();
}

void simulation() {
  // Initialize leaderboard with the initial state
#pragma omp parallel for reduction(+ : leaderboard[ : N_SPECIES + 1])
  for (int i = 0; i < num_blocks; i++) {
    leaderboard[grid[i]]++;
  }

  receiveLeaderboards();
  if (me == 0) {
    updateMaxScores(0);
  }

  MPI_Barrier(comm_cart);

  // generations start at 1
  for (int gen = 1; gen < genNum + 1; gen++) {
    exchangeMessages();

#pragma omp parallel for reduction(+ : leaderboard[ : N_SPECIES + 1])
    for (int z = 1; z < z_size + 1; z++) {
      for (int y = 1; y < y_size + 1; y++) {
        for (int x = 1; x < x_size + 1; x++) {
          int index = z * (x_size + 2) * (y_size + 2) + y * (x_size + 2) + x;
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
    int w = (z - 1) * y_size * x_size + (y - 1) * x_size + (x - 1);
    grid[w] = new_state;
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

  int z_disp = (x_size + 2) * (y_size + 2);
  int y_disp = x_size + 2;

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
  int z_disp = (x_size + 2) * (y_size + 2);
  int y_disp = x_size + 2;

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
