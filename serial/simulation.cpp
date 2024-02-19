#include "simulation.h"
#include "utils.h"

Cell ***grid;
int gridSize;

void simulation(Cell ****g, int genNum, int size) {
  grid = *g;
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

void debugPrintGrid(bool even_gen) {
  for (int x = 0; x < gridSize; x++) {
    for (int y = 0; y < gridSize; y++) {
      for (int z = 0; z < gridSize; z++) {
        int valueToPrint = even_gen ? (int)grid[x][y][z].rightState
                                    : (int)grid[x][y][z].leftState;
        if (valueToPrint == 0) {
          std::cout << "  ";
        } else {
          std::cout << valueToPrint << " ";
        }
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  std::cout << "---" << std::endl;
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

  char current_state = readCellState(x, y, z, even_gen);

  char new_state = calculateNextState(x, y, z, current_state != 0, even_gen);

  writeCellState(x, y, z, even_gen, new_state);

  writeToLeaderboard(new_state);
};

// odd states will read the lower 4 bits, even states will read the upper 4 bits
char readCellState(int x, int y, int z, bool even_gen) {
  return even_gen ? grid[x][y][z].leftState : grid[x][y][z].rightState;
};

// odd states will write the upper 4 bits, even states will write the lower 4
// bits
void writeCellState(int x, int y, int z, bool even_gen, char new_state) {
  if (even_gen) {
    grid[x][y][z].rightState = new_state;
  } else {
    grid[x][y][z].leftState = new_state;
  }
};

// wraps around the grid
char calculateNextState(int x, int y, int z, bool alive, bool even_gen) {
  int aliveCounter = 0;
  char neighborsValues[NUM_TYPE_ALIVE] = {0};
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      for (int k = -1; k < 2; k++) {
        unsigned char value = readCellState(
            (x + i + gridSize) % gridSize, (y + j + gridSize) % gridSize,
            (z + k + gridSize) % gridSize, even_gen);

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

char getMostFrequentValue(char *neighborsValues) {
  char mostFrequentValue = 0;
  int maxCount = 0;
  for (int i = 0; i < NUM_TYPE_ALIVE; i++) {
    if (neighborsValues[i] > maxCount) {
      maxCount = neighborsValues[i];
      mostFrequentValue = i;
    }
  }
  return mostFrequentValue + 1;
};
