#include "simulation.h"

#include <string.h>

#include <omp.h>
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

#pragma omp parallel for
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

void commitState() {
  memcpy(cube->cache, auxNeighbours,
         gridSize * gridSize * gridSize * sizeof(unsigned char));

  memcpy(cube->grid, auxState,
         gridSize * gridSize * gridSize * sizeof(unsigned char));
}

void simulation() {
  // generations start at 1
  for (int i = 1; i < genNum + 1; i++) {

    clearLeaderboard();

    updateGridState();

    updateMaxScores(i);

    commitState();
  }
};

void updateGridState() {
#pragma omp parallel for
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

unsigned char readCellState(int index) { return cube->grid[index]; };

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

  int z1 = (z + gridSize - 1) % gridSize * gridSize * gridSize;
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

void debugPrintGrid(unsigned char *grid) {
  fprintf(stdout, "\nDebug---\n");

  for (int z = 0; z < gridSize; z++) {
    for (int y = 0; y < gridSize; y++) {
      for (int x = 0; x < gridSize; x++) {
        int index = z * gridSize * gridSize + y * gridSize + x;
        int valueToPrint = grid[index];
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
