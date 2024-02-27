#include "simulation.h"

#include <string.h>

#include <time.h>

Cube *cube;
int gridSize;
unsigned char *auxState;
unsigned char *auxNeighbours;
int genNum;

void initializeAux(Cube *c, int num, int size) {
  cube = c;
  gridSize = size;
  genNum = num;

  auxNeighbours = (unsigned char *)malloc(gridSize * gridSize * gridSize *
                                          sizeof(unsigned char));

  auxState = (unsigned char *)malloc(gridSize * gridSize * gridSize *
                                     sizeof(unsigned char));

  memcpy(auxNeighbours, cube->cache,
         gridSize * gridSize * gridSize * sizeof(unsigned char));

  for (int z = 0; z < gridSize; z++) {
    for (int y = 0; y < gridSize; y++) {
      for (int x = 0; x < gridSize; x++) {
        int index = z * gridSize * gridSize + y * gridSize + x;
        writeToLeaderboard(readCellState(index));
      }
    }
  }

  updateMaxScores(0);
}

void simulation() {
  // generations start at 1
  for (int i = 1; i < genNum + 1; i++) {

    clearLeaderboard();

    updateGridState();

    updateMaxScores(i);

    memcpy(cube->cache, auxNeighbours,
           gridSize * gridSize * gridSize * sizeof(unsigned char));

    memcpy(cube->grid, auxState,
           gridSize * gridSize * gridSize * sizeof(unsigned char));
  }
};

void updateGridState() {
  for (int z = 0; z < gridSize; z++) {
    for (int y = 0; y < gridSize; y++) {
      for (int x = 0; x < gridSize; x++) {
        updateCellState(x, y, z);
      }
    }
  }
};

void updateCellState(int x, int y, int z) {

  int index = z * gridSize * gridSize + y * gridSize + x;

  unsigned char current_state = readCellState(index);
  unsigned char new_state = calculateNextState(x, y, z, current_state, index);

  writeCellState(x, y, z, index, current_state, new_state);

  writeToLeaderboard(new_state);
};

unsigned char readCellState(int index) { return GET_CELL_INDEX(cube, index); };

void writeCellState(int x, int y, int z, int index, unsigned char old_state,
                    unsigned char new_state) {

  auxState[index] = new_state;

  if (old_state != new_state) {
    updateNeighborsCount(auxNeighbours, gridSize, x, y, z,
                         new_state == 0 ? -1 : 1);
  }
};

// wraps around the grid
unsigned char calculateNextState(int x, int y, int z,
                                 unsigned char current_state, int index) {

  unsigned char neighbourCount = cube->cache[index];
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
    int z_ = (z + k + gridSize) % gridSize * gridSize * gridSize;
    for (int j = -1; j <= 1; j++) {
      int y_ = (y + j + gridSize) % gridSize * gridSize;
      for (int i = -1; i <= 1; i++) {
        if (k == 0 && j == 0 && i == 0) {
          continue;
        }
        int x_ = (x + i + gridSize) % gridSize;
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
  for (int z = 0; z < gridSize; z++) {
    for (int y = 0; y < gridSize; y++) {
      for (int x = 0; x < gridSize; x++) {
        int index = z * gridSize * gridSize + y * gridSize + x;
        int valueToPrint = (int)cube->grid[index];
        if (valueToPrint == 0) {
          fprintf(stdout, "  ");
        } else {
          fprintf(stdout, "%d ", valueToPrint);
        }
      }

      fprintf(stdout, "\n");
    }

    fprintf(stdout, "\n");
  }

  fprintf(stdout, "---\n");
};
