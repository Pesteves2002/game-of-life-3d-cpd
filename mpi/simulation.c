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
  auxState = (unsigned char *)malloc(num_blocks * sizeof(unsigned char));

  memcpy(auxState, grid,
         num_blocks *
             sizeof(unsigned char)); // copy the initial state to auxState

  int area_xy = x_size * y_size;
  int area_xz = x_size * z_size;
  int area_yz = y_size * z_size;
};

void exchangeX() {
  int neg_x_rank, pos_x_rank;

  if (MPI_Cart_shift(comm_cart, 0, 1, &neg_x_rank, &pos_x_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  int area_zy = z_size * y_size;

  unsigned char *payload_neg_x =
      (unsigned char *)malloc(area_zy * sizeof(unsigned char));
  unsigned char *payload_pos_x =
      (unsigned char *)malloc(area_zy * sizeof(unsigned char));

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
      (unsigned char *)malloc(area_zy * sizeof(unsigned char));
  unsigned char *pos_x =
      (unsigned char *)malloc(area_zy * sizeof(unsigned char));

  MPI_Request requests[4];
  MPI_Status statuses[4];

  MPI_Irecv(neg_x, area_zy, MPI_UNSIGNED_CHAR, neg_x_rank, 2, comm_cart,
            &requests[0]);
  MPI_Irecv(pos_x, area_zy, MPI_UNSIGNED_CHAR, pos_x_rank, 1, comm_cart,
            &requests[1]);

  MPI_Isend(payload_neg_x, area_zy, MPI_UNSIGNED_CHAR, neg_x_rank, 1, comm_cart,
            &requests[2]);
  MPI_Isend(payload_pos_x, area_zy, MPI_UNSIGNED_CHAR, pos_x_rank, 2, comm_cart,
            &requests[3]);

  MPI_Waitall(4, requests, statuses);

  int aux_x_size = (2 + x_size) * y_size * z_size;
  aux_x = (unsigned char *)malloc(aux_x_size * sizeof(unsigned char));

  memset(aux_x, 10, aux_x_size * sizeof(unsigned char));
  for (int i = 0; i < area_zy; i++) {
    aux_x[i * (2 + x_size)] = neg_x[i];
    aux_x[i * (2 + x_size) + x_size + 1] = pos_x[i];
  }

  int index = 0;
  // TODO: verify condition
  for (int i = 0; i < num_blocks; i++) {
    while (aux_x[index] != 10) {
      index++;
    }
    aux_x[index] = grid[i];
  }

  if (me == 0) {
    for (int i = 0; i < aux_x_size; i++) {
      printf("%d ", aux_x[i]);
      if ((i + 1) % (2 + x_size) == 0) {
        printf("\n");
      }
      if (i == aux_x_size / 2 - 1) {
        printf("\n");
      }
    }
    printf("\n");
    printf("\n");
  }
}

void exchangeY() {
  int neg_y_rank, pos_y_rank;

  if (MPI_Cart_shift(comm_cart, 1, 1, &neg_y_rank, &pos_y_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  int area_xz = (x_size + 2) * z_size; // +2 accounts for received x cells

  unsigned char *payload_neg_y =
      (unsigned char *)malloc(area_xz * sizeof(unsigned char));
  unsigned char *payload_pos_y =
      (unsigned char *)malloc(area_xz * sizeof(unsigned char));

  int aux_x_size = (2 + x_size) * y_size * z_size;
  memcpy(payload_neg_y, aux_x, area_xz * sizeof(unsigned char));
  memcpy(payload_pos_y, aux_x + (aux_x_size - area_xz),
         area_xz * sizeof(unsigned char));

  unsigned char *neg_y =
      (unsigned char *)malloc(area_xz * sizeof(unsigned char));

  unsigned char *pos_y =
      (unsigned char *)malloc(area_xz * sizeof(unsigned char));

  MPI_Request requests[4];
  MPI_Status statuses[4];

  MPI_Irecv(neg_y, area_xz, MPI_UNSIGNED_CHAR, neg_y_rank, 2, comm_cart,
            &requests[0]);
  MPI_Irecv(pos_y, area_xz, MPI_UNSIGNED_CHAR, pos_y_rank, 1, comm_cart,
            &requests[1]);

  MPI_Isend(payload_neg_y, area_xz, MPI_UNSIGNED_CHAR, neg_y_rank, 1, comm_cart,
            &requests[2]);
  MPI_Isend(payload_pos_y, area_xz, MPI_UNSIGNED_CHAR, pos_y_rank, 2, comm_cart,
            &requests[3]);

  MPI_Waitall(4, requests, statuses);

  int aux_y_size = (2 + x_size) * (2 + y_size) * z_size;
  aux_y = (unsigned char *)malloc(aux_y_size * sizeof(unsigned char));

  memset(aux_y, 10, aux_y_size * sizeof(unsigned char));
  for (int i = 0; i < area_xz / 2; i++) {
    aux_y[i] = pos_y[i + area_xz / 2];
    aux_y[i + area_xz + x_size + 2] = pos_y[i];

    aux_y[i + aux_y_size / 2] = neg_y[i + area_xz / 2];
    aux_y[i + aux_y_size / 2 + area_xz + x_size + 2] = neg_y[i];
  }

  int index = 0;
  // TODO: verify condition
  for (int i = 0; i < num_blocks * 2; i++) {
    while (aux_y[index] != 10) {
      index++;
    }
    aux_y[index] = aux_x[i];
  }

  if (me == 0) {
    for (int i = 0; i < aux_y_size; i++) {
      printf("%d ", aux_y[i]);
      if ((i + 1) % (2 + x_size) == 0) {
        printf("\n");
      }
      if (i == aux_y_size / 2 - 1) {
        printf("\n");
      }
    }
  }
}

void exchangeZ() {
  int neg_z_rank, pos_z_rank;

  if (MPI_Cart_shift(comm_cart, 2, 1, &neg_z_rank, &pos_z_rank) !=
      MPI_SUCCESS) {
    fprintf(stderr, "MPI Cart_shift error\n");
    return;
  }

  int area_xy =
      (x_size + 2) * (y_size + 2); // +2 accounts for received x and y cells
                                   // TODO: check if this is correct

  unsigned char *payload_neg_z =
      (unsigned char *)malloc(area_xy * sizeof(unsigned char));
  unsigned char *payload_pos_z =
      (unsigned char *)malloc(area_xy * sizeof(unsigned char));

  int count_pos = 0;
  int count_neg = 0;

  // TODO: add the cells to be sent to the payload

  unsigned char *neg_z =
      (unsigned char *)malloc(area_xy * sizeof(unsigned char));
  unsigned char *pos_z =
      (unsigned char *)malloc(area_xy * sizeof(unsigned char));

  MPI_Request requests[4];
  MPI_Status statuses[4];

  MPI_Irecv(neg_z, area_xy, MPI_UNSIGNED_CHAR, neg_z_rank, 2, comm_cart,
            &requests[0]);
  MPI_Irecv(pos_z, area_xy, MPI_UNSIGNED_CHAR, pos_z_rank, 1, comm_cart,
            &requests[1]);

  MPI_Isend(payload_neg_z, area_xy, MPI_UNSIGNED_CHAR, neg_z_rank, 1, comm_cart,
            &requests[2]);
  MPI_Isend(payload_pos_z, area_xy, MPI_UNSIGNED_CHAR, pos_z_rank, 2, comm_cart,
            &requests[3]);

  MPI_Waitall(4, requests, statuses);

  // TODO: check if the received cells are correct
}

void exchangeMessages() {
  exchangeX();
  exchangeY();
  exchangeZ();
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
