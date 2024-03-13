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

int z_size = 1;
int y_size = 1;
int x_size = 1;

unsigned char *pos_x;
unsigned char *neg_x;
unsigned char *pos_y;
unsigned char *neg_y;
unsigned char *pos_z;
unsigned char *neg_z;

void initializeAux(unsigned char *g, int num, int size, int m, int procs,
                   MPI_Comm comm) {
  grid = g;
  grid_size = size;
  genNum = num;
  me = m;
  nprocs = procs;
  comm_cart = comm;

  if (MPI_Cart_coords(comm_cart, me, 3, coords) != MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_coords error\n");
    return;
  }
  printf("coords: %d %d %d\n", coords[0], coords[1], coords[2]);

  num_blocks = grid_size * grid_size * grid_size / nprocs;
  int total_aux = num_blocks;

  // Calculate the size of the z dimension
  while (num_blocks / z_size > grid_size * grid_size) {
    z_size++;
  }

  num_blocks /= z_size;

  // Calculate the size of the y dimension
  while (num_blocks / y_size > grid_size) {
    y_size++;
  }

  num_blocks /= y_size;

  x_size = num_blocks;

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
  int pos_x_rank, neg_x_rank, pos_y_rank, neg_y_rank, pos_z_rank, neg_z_rank;

  if (MPI_Cart_shift(comm_cart, 0, 1, &neg_x_rank, &pos_x_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  int area_xy = x_size * y_size * sizeof(unsigned char);

  MPI_Request requests[4];
  MPI_Status statuses[4];
  MPI_Irecv(neg_x, area_xy, MPI_UNSIGNED_CHAR, neg_x_rank, 0, comm_cart,
            &requests[0]);
  MPI_Irecv(pos_x, area_xy, MPI_UNSIGNED_CHAR, pos_x_rank, 0, comm_cart,
            &requests[1]);
  MPI_Isend(grid, area_xy, MPI_UNSIGNED_CHAR, neg_x_rank, 0, comm_cart,
            &requests[2]);
  MPI_Isend(grid, area_xy, MPI_UNSIGNED_CHAR, pos_x_rank, 0, comm_cart,
            &requests[3]);

  MPI_Waitall(4, requests, statuses);

  fprintf(stdout, "Rank: %d, neg_x: ", me);
  for (int i = 0; i < area_xy; i++) {
    fprintf(stdout, "%d ", neg_x[i]);
  }
  fprintf(stdout, "\n");
  for (int i = 0; i < area_xy; i++) {
    fprintf(stdout, "%d ", pos_x[i]);
  }
  fprintf(stdout, "\n");
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
