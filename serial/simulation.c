#include "simulation.h"

Cube *cube;
int gridSize;

void simulation(Cube *c, int genNum, int size) {
  cube = c;
  gridSize = size;

  for (int x = 0; x < gridSize; x++) {
    for (int y = 0; y < gridSize; y++) {
      for (int z = 0; z < gridSize; z++) {
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
  }
};

void updateGridState(bool even_gen) {
  for (int x = 0; x < gridSize; x++) {
    for (int y = 0; y < gridSize; y++) {
      for (int z = 0; z < gridSize; z++) {
        updateCellState(x, y, z, even_gen);
      }
    }
  }
};

void updateCellState(int x, int y, int z, bool even_gen) {

  unsigned char current_state = readCellState(x, y, z, even_gen);

  unsigned char new_state =
      calculateNextState(x, y, z, current_state != 0, even_gen);

  writeCellState(x, y, z, even_gen, new_state);

  writeToLeaderboard(new_state);
};

// odd states will read the lower 4 bits, even states will read the upper 4 bits
unsigned char readCellState(int x, int y, int z, bool even_gen) {
  return even_gen ? GET_CELL(cube, x, y, z).leftState
                  : GET_CELL(cube, x, y, z).rightState;
};

// odd states will write the upper 4 bits, even states will write the lower 4
// bits
void writeCellState(int x, int y, int z, bool even_gen,
                    unsigned char new_state) {
  if (even_gen) {
    SET_CELL_RIGHT_STATE(cube, x, y, z, new_state);
  } else {
    SET_CELL_LEFT_STATE(cube, x, y, z, new_state);
  }
};

// wraps around the grid
unsigned char calculateNextState(int x, int y, int z, bool alive,
                                 bool even_gen) {
  int aliveCounter = 0;
  unsigned char neighborsValues[N_SPECIES] = {0};
  for (int k = -1; k <= 1; k++) {
    int z_ = (z + k + gridSize) % gridSize;
    for (int j = -1; j <= 1; j++) {
      int y_ = (y + j + gridSize) % gridSize;
      for (int i = -1; i <= 1; i++) {
        int x_ = (x + i + gridSize) % gridSize;
        unsigned char value = readCellState(x_, y_, z_, even_gen);

        if (value != 0) {
          neighborsValues[value - 1]++;
          aliveCounter++;
        }
      }
    }
  }
  if (!alive)
    return aliveCounter >= 7 && aliveCounter <= 10
               ? getMostFrequentValue(neighborsValues)
               : 0;

  // since the cell is alive, it will count itself
  // we will need to subtract 1 from the aliveCounter
  aliveCounter--;
  return (aliveCounter <= 4 || aliveCounter > 13)
             ? 0
             : readCellState(x, y, z, even_gen);
};

unsigned char getMostFrequentValue(unsigned char *neighborsValues) {
  unsigned char mostFrequentValue = 0;
  int maxCount = 0;
  for (int i = 0; i < N_SPECIES; i++) {
    if (neighborsValues[i] > maxCount) {
      maxCount = neighborsValues[i];
      mostFrequentValue = i;
    }
  }
  return mostFrequentValue + 1;
};

void debugPrintGrid(bool even_gen) {
  for (int x = 0; x < gridSize; x++) {
    for (int y = 0; y < gridSize; y++) {
      for (int z = 0; z < gridSize; z++) {
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
