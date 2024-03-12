#include "simulation.h"

unsigned char *grid;
int gridSize;
int gridPadding;
unsigned char *auxState;
int genNum;

long long leaderboard[(N_SPECIES + 1) * 3] = {0}; // current, max, max gen

void initializeAux(unsigned char *g, int num, int size) {
  grid = g;
  gridSize = size;
  gridPadding = size + 2;
  genNum = num;

  auxState = (unsigned char *)malloc(gridPadding * gridPadding * gridPadding *
                                     sizeof(unsigned char));

  memcpy(auxState, grid,
         gridPadding * gridPadding * gridPadding * sizeof(unsigned char));
};

void simulation(){

#pragma omp parallel
    {
#pragma omp for reduction(+ : leaderboard[ : N_SPECIES + 1])
        for (int z = 1; z < gridPadding - 1;
             z++){int z_ = z * gridPadding * gridPadding;
for (int y = 1; y < gridPadding - 1; y++) {
  int y_ = y * gridPadding;
  for (int x = 1; x < gridPadding - 1; x++) {
    leaderboard[grid[z_ + y_ + x]]++;
  }
}
}

#pragma omp single
updateMaxScores(0);

// generations start at 1
for (int gen = 1; gen < genNum + 1; gen++) {

#pragma omp for reduction(+ : leaderboard[ : N_SPECIES + 1])
  for (int z = 1; z < gridPadding - 1; z++) {
    int z_ = z * gridPadding * gridPadding;
    for (int y = 1; y < gridPadding - 1; y++) {
      int y_ = y * gridPadding;
      for (int x = 1; x < gridPadding - 1; x++) {
        leaderboard[updateCellState(x, y, z, z_ + y_ + x)]++;
      }
    }
  }

#pragma omp single nowait
  updateMaxScores(gen);

#pragma omp single
  memcpy(grid, auxState,
         gridPadding * gridPadding * gridPadding * sizeof(unsigned char));
}
}
}
;

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

