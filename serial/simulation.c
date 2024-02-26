#include "simulation.h"

#include <string.h>

#include <time.h>

Cube *cube;
int gridSize;
unsigned char *aux;

void simulation(Cube *c, int genNum, int size) {
  cube = c;
  gridSize = size;

  aux = (unsigned char *)malloc(gridSize * gridSize * gridSize *
                                sizeof(unsigned char));
  memcpy(aux, cube->cache,
         gridSize * gridSize * gridSize * sizeof(unsigned char));

  for (int z = 0; z < gridSize; z++) {
    for (int y = 0; y < gridSize; y++) {
      for (int x = 0; x < gridSize; x++) {
        writeToLeaderboard(readCellState(x, y, z, 0));
      }
    }
  }

  updateMaxScores(0);

  // generations start at 1
  for (int i = 1; i < genNum + 1; i++) {

    clearLeaderboard();

    updateGridState(i % 2 == 0);

    updateMaxScores(i);

    memcpy(cube->cache, aux,
           gridSize * gridSize * gridSize * sizeof(unsigned char));
  }
};

void updateGridState(bool even_gen) {
  for (int z = 0; z < gridSize; z++) {
    for (int y = 0; y < gridSize; y++) {
      for (int x = 0; x < gridSize; x++) {
        updateCellState(x, y, z, even_gen);
      }
    }
  }
};

void updateCellState(int x, int y, int z, bool even_gen) {

  // unsigned char current_state = readCellState(x, y, z, even_gen);

  int index = z * gridSize * gridSize + y * gridSize + x;
  Cell *cell = GET_CELL_INDEX(&cube, index);

  unsigned char current_state = even_gen ? cell->leftState : cell->rightState;
  unsigned char new_state =
      calculateNextState(x, y, z, current_state, index, even_gen);

  writeCellState(x, y, z, cell, even_gen, current_state, new_state);

  writeToLeaderboard(new_state);
};

// odd states will read the lower 4 bits, even states will read the upper 4 bits
unsigned char readCellState(int x, int y, int z, bool even_gen) {
  return even_gen ? GET_CELL(cube, x, y, z).leftState
                  : GET_CELL(cube, x, y, z).rightState;
};

// odd states will write the upper 4 bits, even states will write the lower 4
// bits
void writeCellState(int x, int y, int z, Cell *cell, bool even_gen,
                    unsigned char old_state, unsigned char new_state) {
  if (even_gen) {
    cell->rightState = new_state;
  } else {
    cell->leftState = new_state;
  }

  if (old_state != new_state) {
    updateNeighborsCount(aux, gridSize, x, y, z, new_state == 0 ? -1 : 1);
  }
};

// wraps around the grid
unsigned char calculateNextState(int x, int y, int z,
                                 unsigned char current_state, int index,
                                 bool even_gen) {

  unsigned char neighbourCount = cube->cache[index];
  if (current_state == 0) {

    if (neighbourCount < 7 || neighbourCount > 10) {
      return 0;
    }

    unsigned char neighborsValues[N_SPECIES + 1] = {0};
    getNeighborsValue(x, y, z, even_gen, neighborsValues);
    return getMostFrequentValue(neighborsValues);
  }

  return (neighbourCount <= 4 || neighbourCount > 13) ? 0 : current_state;
};

void getNeighborsValue(int x, int y, int z, bool even_gen,
                       unsigned char *neighborsValues) {
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
        Cell *cell = GET_CELL_INDEX(&cube, index);
        unsigned char value = even_gen ? cell->leftState : cell->rightState;
        neighborsValues[value]++;
      }
    }
  }
};

unsigned char getMostFrequentValue(unsigned char *neighborsValues) {
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

void debugPrintGrid(bool even_gen) {
  for (int z = 0; z < gridSize; z++) {
    for (int y = 0; y < gridSize; y++) {
      for (int x = 0; x < gridSize; x++) {
        int valueToPrint = even_gen ? (int)GET_CELL(cube, x, y, z).rightState
                                    : (int)GET_CELL(cube, x, y, z).leftState;
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
