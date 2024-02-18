#include "simulation.h"

void simulation() {
  initializeLeaderboard();

  // generations start at 1
  for (int i = 1; i < gen_num + 2; i++) {
    clearCurrentLeaderboard();

    debugPrintGrid(i % 2 == 0);

    updateGridState(i % 2 == 0);

    // updateMaxLeaderboard();
  }

  printLeaderboard();
};

void debugPrintGrid(bool even_gen) {
  for (int x = 0; x < grid_size; x++) {
    for (int y = 0; y < grid_size; y++) {
      for (int z = 0; z < grid_size; z++) {
        int valueToPrint = even_gen ? (int)((grid[x][y][z] >> 4) & 0x0F)
                                    : (int)((grid[x][y][z] & 0x0F));
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

void initializeLeaderboard() {
  for (int i = 0; i < 20; i++) {
    leaderboard[i] = 0;
  }
};

void clearCurrentLeaderboard() {
  for (int i = 0; i < 9; i++) {
    leaderboard[i] = 0;
  }
};

void updateMaxLeaderboard() {
  for (int i = 0; i < 9; i++) {
    if (leaderboard[i] > leaderboard[i + 9]) {
      leaderboard[i + 9] = leaderboard[i];
    }
  }
};

void printLeaderboard() {
  for (int i = 0; i < 9; i++) {
    std::cout << i + 1 << " " << leaderboard[i + 9] << " " << leaderboard[i]
              << std::endl;
  }
};

void writeToLeaderboard(char new_state) {
  if (new_state == 0) {
    return;
  }
  leaderboard[new_state - 1]++;
};

void updateGridState(bool even_gen) {
  for (int x = 0; x < grid_size; x++) {
    for (int y = 0; y < grid_size; y++) {
      for (int z = 0; z < grid_size; z++) {
        updateCellState(x, y, z, even_gen);
      }
    }
  }
};

void updateCellState(int x, int y, int z, bool even_gen) {

  char current_state = readCellState(x, y, z, even_gen);

  char new_state = calculateNextState(x, y, z, current_state != 0, even_gen);

  writeCellState(x, y, z, even_gen, new_state);

  // writeToLeaderboard(new_state);
};

// odd states will read the lower 4 bits, even states will read the upper 4 bits
char readCellState(int x, int y, int z, bool even_gen) {
  return even_gen ? ((grid[x][y][z] >> 4) & 0x0F) : (grid[x][y][z] & 0x0F);
};

// odd states will write the upper 4 bits, even states will write the lower 4
// bits
void writeCellState(int x, int y, int z, bool even_gen, char new_state) {
  if (even_gen) {
    grid[x][y][z] = (grid[x][y][z] & 0xF0) | new_state;
  } else {
    grid[x][y][z] = (grid[x][y][z] & 0x0F) | (new_state << 4);
  }
};

// wraps around the grid
char calculateNextState(int x, int y, int z, bool alive, bool even_gen) {
  int aliveCounter = 0;
  std::map<char, int> neighborsValues;
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      for (int k = -1; k < 2; k++) {
        if (i == 0 && j == 0 && k == 0) {
          continue;
        }

        char value = readCellState((x + i + grid_size) % grid_size,
                                   (y + j + grid_size) % grid_size,
                                   (z + k + grid_size) % grid_size, even_gen);

        if (value != 0) {
          neighborsValues[value]++;
          aliveCounter++;
        }
      }
    }
  }
  if (!alive)
    return aliveCounter >= 7 && aliveCounter <= 10
               ? getMostFrequentValue(neighborsValues)
               : 0;

  if (aliveCounter <= 4 || aliveCounter > 13) {
    return 0;
  }

  return readCellState(x, y, z, even_gen);
};

char getMostFrequentValue(std::map<char, int> neighborsValues) {
  char mostFrequentValue = 0;
  int maxCount = 0;
  for (auto const &pair : neighborsValues) {
    if (pair.second > maxCount) {
      maxCount = pair.second;
      mostFrequentValue = pair.first;
    }
  }
  return mostFrequentValue;
};
