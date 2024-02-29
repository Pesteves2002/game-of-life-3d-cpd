#include "simulation.h"

#include <string.h>

Cube *cube;
int gridSize;
int gridPadding;
unsigned char *auxState;
unsigned char *auxNeighbourCount;
int genNum;

void initializeAux(Cube *c, int num, int size) {
  cube = c;
  gridSize = size;
  gridPadding = size + 2;
  genNum = num;

  auxState = (unsigned char *)malloc(gridPadding * gridPadding * gridPadding *
                                     sizeof(unsigned char));

  auxNeighbourCount = (unsigned char *)malloc(
      gridPadding * gridPadding * gridPadding * sizeof(unsigned char));

  for (int z = 1; z < gridPadding - 1; z++) {
    for (int y = 1; y < gridPadding - 1; y++) {
      for (int x = 1; x < gridPadding - 1; x++) {
        int index = z * gridPadding * gridPadding + y * gridPadding + x;
        writeToLeaderboard(readCellState(index));
      }
    }
  }

  updateMaxScores(0);

  memcpy(auxState, cube->grid,
         gridPadding * gridPadding * gridPadding * sizeof(unsigned char));

  memcpy(auxNeighbourCount, cube->neighbourCount,
         gridPadding * gridPadding * gridPadding * sizeof(unsigned char));
};

void simulation() {
  // generations start at 1
  for (int gen = 1; gen < genNum + 1; gen++) {
    clearLeaderboard();

    updateGridState();

    updateMaxScores(gen);

    updateMaxScores(gen);

    memcpy(cube->grid, auxState,
           gridPadding * gridPadding * gridPadding * sizeof(unsigned char));

    memcpy(cube->neighbourCount, auxNeighbourCount,
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

  writeCellState(x, y, z, index, current_state, new_state);

  writeToLeaderboard(new_state);
};

unsigned char readCellState(int index) { return cube->grid[index]; };

void writeCellState(int x, int y, int z, int index, unsigned char old_state,
                    unsigned char value) {

  auxState[index] = value;
  bool border_x = x == 1 || x == gridPadding - 2;
  bool border_y = y == 1 || y == gridPadding - 2;
  bool border_z = z == 1 || z == gridPadding - 2;

  int x_ = x == 1 ? gridPadding - 1 : 0;
  int y_ = y == 1 ? gridPadding - 1 : 0;
  int z_ = z == 1 ? gridPadding - 1 : 0;

  if (border_x) {
    index = z * gridPadding * gridPadding + y * gridPadding + x_;
    auxState[index] = value;
  }

  if (border_y) {
    index = z * gridPadding * gridPadding + y_ * gridPadding + x;
    auxState[index] = value;
  }

  if (border_z) {
    index = z_ * gridPadding * gridPadding + y * gridPadding + x;
    auxState[index] = value;
  }

  if (border_x && border_y) {
    index = z * gridPadding * gridPadding + y_ * gridPadding + x_;
    auxState[index] = value;
  }

  if (border_x && border_z) {
    index = z_ * gridPadding * gridPadding + y * gridPadding + x_;
    auxState[index] = value;
  }

  if (border_y && border_z) {
    index = z_ * gridPadding * gridPadding + y_ * gridPadding + x;
    auxState[index] = value;
  }

  if (border_x && border_y && border_z) {
    index = z_ * gridPadding * gridPadding + y_ * gridPadding + x_;
    auxState[index] = value;
  }

  if (old_state != value) {
    updateNeighborsCount(auxNeighbourCount, gridPadding, x, y, z,
                         value == 0 ? -1 : 1);
  }
};

// wraps around the grid
unsigned char calculateNextState(int x, int y, int z,
                                 unsigned char current_state, int index) {

  unsigned char neighbourCount = cube->neighbourCount[index];
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

  /*  int z1 = (z + gridSize - 1) % gridSize * gridSize * gridSize;
   int z2 = (z + gridSize) % gridSize * gridSize * gridSize;
   int z3 = (z + 1) % gridSize * gridSize * gridSize;
   int y1 = (y + gridSize - 1) % gridSize * gridSize;
   int y2 = (y + gridSize) % gridSize * gridSize;
   int y3 = (y + 1) % gridSize * gridSize;
   int x1 = (x + gridSize - 1) % gridSize;
   int x2 = (x + gridSize) % gridSize;
   int x3 = (x + 1) % gridSize;

   neighborsValues[cube->grid[z1 + y1 + x1]]++;
   neighborsValues[cube->grid[z1 + y1 + x2]]++;
   neighborsValues[cube->grid[z1 + y1 + x3]]++;
   neighborsValues[cube->grid[z1 + y2 + x1]]++;
   neighborsValues[cube->grid[z1 + y2 + x2]]++;
   neighborsValues[cube->grid[z1 + y2 + x3]]++;
   neighborsValues[cube->grid[z1 + y3 + x1]]++;
   neighborsValues[cube->grid[z1 + y3 + x2]]++;
   neighborsValues[cube->grid[z1 + y3 + x3]]++;

   neighborsValues[cube->grid[z2 + y1 + x1]]++;
   neighborsValues[cube->grid[z2 + y1 + x2]]++;
   neighborsValues[cube->grid[z2 + y1 + x3]]++;
   neighborsValues[cube->grid[z2 + y2 + x1]]++;
   //  neighborsValues[cube->grid[z2 + y2 + x2]]++;
   neighborsValues[cube->grid[z2 + y2 + x3]]++;
   neighborsValues[cube->grid[z2 + y3 + x1]]++;
   neighborsValues[cube->grid[z2 + y3 + x2]]++;
   neighborsValues[cube->grid[z2 + y3 + x3]]++;

   neighborsValues[cube->grid[z3 + y1 + x1]]++;
   neighborsValues[cube->grid[z3 + y1 + x2]]++;
   neighborsValues[cube->grid[z3 + y1 + x3]]++;
   neighborsValues[cube->grid[z3 + y2 + x1]]++;
   neighborsValues[cube->grid[z3 + y2 + x2]]++;
   neighborsValues[cube->grid[z3 + y2 + x3]]++;
   neighborsValues[cube->grid[z3 + y3 + x1]]++;
   neighborsValues[cube->grid[z3 + y3 + x2]]++;
   neighborsValues[cube->grid[z3 + y3 + x3]]++;

    */
  unsigned char mostFrequentValue = 1;
  int maxCount = neighborsValues[1];
  for (int i = 2; i < N_SPECIES + 1; i++) {
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

void debugPrintNeighbourCount() {
  for (int z = 0; z < gridPadding; z++) {
    for (int y = 0; y < gridPadding; y++) {
      for (int x = 0; x < gridPadding; x++) {
        int index = z * gridPadding * gridPadding + y * gridPadding + x;
        fprintf(stdout, "%d ", (int)cube->neighbourCount[index]);
      }
      fprintf(stdout, "\n");
    }

    fprintf(stdout, "||||\n");
  }

  fprintf(stdout, "---\n");
};

void commitState() {
  memcpy(cube->grid, auxState,
         gridPadding * gridPadding * gridPadding * sizeof(unsigned char));
  memcpy(cube->neighbourCount, auxNeighbourCount,
         gridPadding * gridPadding * gridPadding * sizeof(unsigned char));
};
