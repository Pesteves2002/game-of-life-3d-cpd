#include "simulation.h"

unsigned char *grid;
int gridSize;
int gridPadding;
unsigned char *auxState;
int genNum;
int me;
int nprocs;
MPI_Comm comm_cart;
unsigned char *upState;
unsigned char *downState;
int chunk;

long long leaderboard[(N_SPECIES + 1) * 3] = {0}; // current, max, max gen

void initializeAux(unsigned char *g, int num, int size, int m, int procs,
                   MPI_Comm comm) {
  grid = g;
  gridSize = size;
  gridPadding = size + 2;
  genNum = num;
  me = m;
  nprocs = procs;
  comm_cart = comm;

  chunk = gridSize * gridSize * gridSize / nprocs;
  auxState = (unsigned char *)malloc(chunk * sizeof(unsigned char));

  memcpy(auxState, grid,
         chunk * sizeof(unsigned char)); // copy the initial state to auxState

  upState = (unsigned char *)malloc(chunk * sizeof(unsigned char));
  downState = (unsigned char *)malloc(chunk * sizeof(unsigned char));
};

void exchangeMessages() {
  int up_rank, down_rank;

  MPI_Cart_shift(comm_cart, 0, 1, &up_rank, &down_rank);

  MPI_Request requests[2];
  MPI_Status statuses[2];
  MPI_Irecv(downState, chunk * sizeof(unsigned char), MPI_UNSIGNED_CHAR,
            down_rank, 0, comm_cart, &requests[0]);
  MPI_Irecv(upState, chunk * sizeof(unsigned char), MPI_UNSIGNED_CHAR, up_rank,
            0, comm_cart, &requests[1]);
  MPI_Send(auxState, chunk * sizeof(unsigned char), MPI_UNSIGNED_CHAR, up_rank,
           0, comm_cart);
  MPI_Send(auxState, chunk * sizeof(unsigned char), MPI_UNSIGNED_CHAR,
           down_rank, 0, comm_cart);

  MPI_Waitall(2, requests, statuses);
}

unsigned char getNeighbourCountIndex(int index){};

unsigned char calculateNextStateIndex(int index, int current_state) {
  unsigned char neighbourCount = getNeighbourCountIndex(index);
  if (current_state == 0) {
    if (!(neighbourCount >= 7 && neighbourCount <= 10)) {
      return 0;
    }

    // return getMostFrequentValue(x, y, z);
  }

  return (neighbourCount <= 4 || neighbourCount > 13) ? 0 : current_state;
};

unsigned char updateCellIndex(int index) {
  unsigned char current_state = grid[index];

  unsigned char new_state = calculateNextStateIndex(index, current_state);

  if (current_state != new_state) {
    auxState[index] = new_state;
  }

  return new_state;
}

void simulation() {
  // Initialize leaderboard with the initial state
  for (int i = 0; i < chunk; i++) {
    leaderboard[grid[i]]++;
  }

  updateMaxScores(0);

  // generations start at 1
  for (int gen = 1; gen < genNum + 1; gen++) {
    exchangeMessages();

    for (int i = 0; i < chunk; i++) {
      leaderboard[updateCellIndex(i)]++;
    }

    updateMaxScores(gen);

    memcpy(grid, auxState, chunk * sizeof(unsigned char));

    MPI_Barrier(comm_cart);
  }
};

unsigned char updateCellState(int x, int y, int z, int index) {

  unsigned char current_state = grid[index];

  unsigned char new_state = calculateNextState(x, y, z, current_state, index);

  if (current_state != new_state) {
    writeCellState(x, y, z, index, current_state, new_state);
  }

  return new_state;
};

void writeCellState(int x, int y, int z, int index, unsigned char old_state,
                    unsigned char value) {

  auxState[index] = value;
  writeBorders(auxState, gridPadding, x, y, z, value);
};

unsigned char getNeighbourCount(int x, int y, int z) {
  unsigned char count = 0;

  int z1 = (z - 1) * gridPadding * gridPadding;
  int z2 = z * gridPadding * gridPadding;
  int z3 = (z + 1) * gridPadding * gridPadding;
  int y1 = (y - 1) * gridPadding;
  int y2 = y * gridPadding;
  int y3 = (y + 1) * gridPadding;
  int x1 = x - 1;
  int x2 = x;
  int x3 = x + 1;

  count += (grid[z1 + y1 + x1] != 0);
  count += (grid[z1 + y1 + x2] != 0);
  count += (grid[z1 + y1 + x3] != 0);
  count += (grid[z1 + y2 + x1] != 0);
  count += (grid[z1 + y2 + x2] != 0);
  count += (grid[z1 + y2 + x3] != 0);
  count += (grid[z1 + y3 + x1] != 0);
  count += (grid[z1 + y3 + x2] != 0);
  count += (grid[z1 + y3 + x3] != 0);

  count += (grid[z2 + y1 + x1] != 0);
  count += (grid[z2 + y1 + x2] != 0);
  count += (grid[z2 + y1 + x3] != 0);
  count += (grid[z2 + y2 + x1] != 0);
  count += (grid[z2 + y2 + x3] != 0);
  count += (grid[z2 + y3 + x1] != 0);
  count += (grid[z2 + y3 + x2] != 0);
  count += (grid[z2 + y3 + x3] != 0);

  count += (grid[z3 + y1 + x1] != 0);
  count += (grid[z3 + y1 + x2] != 0);
  count += (grid[z3 + y1 + x3] != 0);
  count += (grid[z3 + y2 + x1] != 0);
  count += (grid[z3 + y2 + x2] != 0);
  count += (grid[z3 + y2 + x3] != 0);
  count += (grid[z3 + y3 + x1] != 0);
  count += (grid[z3 + y3 + x2] != 0);
  count += (grid[z3 + y3 + x3] != 0);

  return count;
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

unsigned char getMostFrequentValue(int x, int y, int z) {
  unsigned char neighborsValues[N_SPECIES + 1] = {0};

  int z1 = (z - 1) * gridPadding * gridPadding;
  int z2 = z * gridPadding * gridPadding;
  int z3 = (z + 1) * gridPadding * gridPadding;
  int y1 = (y - 1) * gridPadding;
  int y2 = y * gridPadding;
  int y3 = (y + 1) * gridPadding;
  int x1 = x - 1;
  int x2 = x;
  int x3 = x + 1;

  neighborsValues[grid[z1 + y1 + x1]]++;
  neighborsValues[grid[z1 + y1 + x2]]++;
  neighborsValues[grid[z1 + y1 + x3]]++;
  neighborsValues[grid[z1 + y2 + x1]]++;
  neighborsValues[grid[z1 + y2 + x2]]++;
  neighborsValues[grid[z1 + y2 + x3]]++;
  neighborsValues[grid[z1 + y3 + x1]]++;
  neighborsValues[grid[z1 + y3 + x2]]++;
  neighborsValues[grid[z1 + y3 + x3]]++;

  neighborsValues[grid[z2 + y1 + x1]]++;
  neighborsValues[grid[z2 + y1 + x2]]++;
  neighborsValues[grid[z2 + y1 + x3]]++;
  neighborsValues[grid[z2 + y2 + x1]]++;
  neighborsValues[grid[z2 + y2 + x3]]++;
  neighborsValues[grid[z2 + y3 + x1]]++;
  neighborsValues[grid[z2 + y3 + x2]]++;
  neighborsValues[grid[z2 + y3 + x3]]++;

  neighborsValues[grid[z3 + y1 + x1]]++;
  neighborsValues[grid[z3 + y1 + x2]]++;
  neighborsValues[grid[z3 + y1 + x3]]++;
  neighborsValues[grid[z3 + y2 + x1]]++;
  neighborsValues[grid[z3 + y2 + x2]]++;
  neighborsValues[grid[z3 + y2 + x3]]++;
  neighborsValues[grid[z3 + y3 + x1]]++;
  neighborsValues[grid[z3 + y3 + x2]]++;
  neighborsValues[grid[z3 + y3 + x3]]++;

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
  for (int z = 0; z < gridPadding; z++) {
    for (int y = 0; y < gridPadding; y++) {
      for (int x = 0; x < gridPadding; x++) {
        int index = z * gridPadding * gridPadding + y * gridPadding + x;
        int valueToPrint = (int)grid[index];
        if (valueToPrint == 0) {
          fprintf(stdout, "  ");
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
