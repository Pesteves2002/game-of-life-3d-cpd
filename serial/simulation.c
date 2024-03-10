#include "simulation.h"

#include <string.h>

#include <time.h>

Cube *cube;
int gridSize;
int gridPadding;
unsigned char *auxState;
int genNum;

void initializeAux(Cube *c, int num, int size) {
  cube = c;
  gridSize = size;
  gridPadding = size + 2;
  genNum = num;

  auxState = (unsigned char *)malloc(gridPadding * gridPadding * gridPadding *
                                     sizeof(unsigned char));

  memcpy(auxState, cube->grid,
         gridPadding * gridPadding * gridPadding * sizeof(unsigned char));
};

void simulation() {
  for (int z = 1; z < gridPadding - 1; z++) {
    for (int y = 1; y < gridPadding - 1; y++) {
      for (int x = 1; x < gridPadding - 1; x++) {
        int index = z * gridPadding * gridPadding + y * gridPadding + x;
        writeToLeaderboard(readCellState(index));
      }
    }
  }
  updateMaxScores(0);

  // generations start at 1
  for (int gen = 1; gen < genNum + 1; gen++) {
    clearLeaderboard();

    updateGridState();

    updateMaxScores(gen);

    memcpy(cube->grid, auxState,
           gridPadding * gridPadding * gridPadding * sizeof(unsigned char));
  }
};

void updateGridState() {
  for (int z = 1; z < gridPadding - 1; z++) {
    for (int y = 1; y < gridPadding - 1; y++) {
      for (int x = 1; x < gridPadding - 1; x++) {
        updateCellState(x, y, z);
      }
    }
  }
};

void updateCellState(int x, int y, int z) {

  int index = z * gridPadding * gridPadding + y * gridPadding + x;

  unsigned char current_state = readCellState(index);

  unsigned char new_state = calculateNextState(x, y, z, current_state, index);

  if (current_state != new_state) {
    writeCellState(x, y, z, index, current_state, new_state);
  }

  writeToLeaderboard(new_state);
};

unsigned char readCellState(int index) { return GET_CELL_INDEX(cube, index); };

void writeCellState(int x, int y, int z, int index, unsigned char old_state,
                    unsigned char value) {

  auxState[index] = value;
  writeBorders(auxState, gridPadding, x, y, z, value);
};

unsigned char getNeighbourCount(int x, int y, int z) {
  unsigned char count = 0;
  for (int k = -1; k <= 1; k++) {
    int z_ = (z + k) * gridPadding * gridPadding;
    for (int j = -1; j <= 1; j++) {
      int y_ = (y + j) * gridPadding;
      for (int i = -1; i <= 1; i++) {
        if (k == 0 && j == 0 && i == 0) {
          continue;
        }
        int x_ = x + i;
        int index = z_ + y_ + x_;
        count += (cube->grid[index] != 0) ? 1 : 0;
      }
    }
  }
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
  for (int k = -1; k <= 1; k++) {
    int z_ = (z + k) * gridPadding * gridPadding;
    for (int j = -1; j <= 1; j++) {
      int y_ = (y + j) * gridPadding;
      for (int i = -1; i <= 1; i++) {
        if (k == 0 && j == 0 && i == 0) {
          continue;
        }
        int x_ = x + i;
        int index = z_ + y_ + x_;
        unsigned char value = cube->grid[index];
        neighborsValues[value]++;
      }
    }
  }

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
        int valueToPrint = (int)cube->grid[index];
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
