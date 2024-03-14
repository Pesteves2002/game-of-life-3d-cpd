#include "simulation.h"

unsigned char *grid;
unsigned char *auxState;
int grid_size;
int genNum;

long long leaderboard[(N_SPECIES + 1) * 3] = {0}; // current, max, max gen

MPI_Comm comm_cart;
int coords[3];
int me;
int nprocs;
int num_blocks;

int z_size, y_size, x_size; // num of blocks in each dimension

unsigned char *pos_x;
unsigned char *neg_x;
unsigned char *pos_y;
unsigned char *neg_y;
unsigned char *pos_z;
unsigned char *neg_z;

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
  auxState = (unsigned char *)malloc(num_blocks * sizeof(unsigned char));

  memcpy(auxState, grid,
         num_blocks *
             sizeof(unsigned char)); // copy the initial state to auxState

  int area_xy = x_size * y_size;
  int area_xz = x_size * z_size;
  int area_yz = y_size * z_size;

  pos_x = (unsigned char *)malloc(area_yz * sizeof(unsigned char));
  neg_x = (unsigned char *)malloc(area_yz * sizeof(unsigned char));
  pos_y = (unsigned char *)malloc(area_xz * sizeof(unsigned char));
  neg_y = (unsigned char *)malloc(area_xz * sizeof(unsigned char));
  pos_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));
  neg_z = (unsigned char *)malloc(area_xy * sizeof(unsigned char));
};

void exchangeMessages() {
  int my_coords[3];
  MPI_Cart_coords(comm_cart, me, 3, my_coords);

  fprintf(stderr, "me: %d, coords: %d %d %d\n", me, my_coords[0], my_coords[1],
          my_coords[2]);

  int neg_x_rank, pos_x_rank;

  if (MPI_Cart_shift(comm_cart, 0, 1, &neg_x_rank, &pos_x_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  int zy_size = z_size * y_size;

  unsigned char *payload_neg_x =
      (unsigned char *)malloc(zy_size * sizeof(unsigned char));
  unsigned char *payload_pos_x =
      (unsigned char *)malloc(zy_size * sizeof(unsigned char));

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

  unsigned char *neg_x =
      (unsigned char *)malloc(zy_size * sizeof(unsigned char));
  unsigned char *pos_x =
      (unsigned char *)malloc(zy_size * sizeof(unsigned char));

  MPI_Request requests[4];
  MPI_Status statuses[4];

  MPI_Irecv(neg_x, zy_size, MPI_UNSIGNED_CHAR, neg_x_rank, 0, comm_cart,
            &requests[0]);
  MPI_Irecv(pos_x, zy_size, MPI_UNSIGNED_CHAR, pos_x_rank, 0, comm_cart,
            &requests[1]);
  MPI_Isend(payload_neg_x, zy_size, MPI_UNSIGNED_CHAR, neg_x_rank, 0, comm_cart,
            &requests[2]);
  MPI_Isend(payload_pos_x, zy_size, MPI_UNSIGNED_CHAR, pos_x_rank, 0, comm_cart,
            &requests[3]);

  MPI_Waitall(4, requests, statuses);
}

void simulation() {
  // Initialize leaderboard with the initial state
  for (int i = 0; i < num_blocks; i++) {
    leaderboard[grid[i]]++;
  }

  updateMaxScores(0);

  // generations start at 1
  for (int gen = 1; gen < genNum + 1; gen++) {
    exchangeMessages();

    updateMaxScores(gen);

    memcpy(grid, auxState, num_blocks * sizeof(unsigned char));

    MPI_Barrier(comm_cart);
  }
};

void debugPrintGrid() {
  for (int i = 0; i < num_blocks; i++) {
    unsigned char current_state = grid[i];
    if (current_state == 0) {
      printf(" ");
    } else {
      printf("%d", current_state);
    }
  }
  printf("\n");
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
