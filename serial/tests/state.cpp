#include "../simulation.h"
#include "../utils.h"

#include <bitset>
#include <cassert>

char ***grid;              // defined in simulation.h
int gen_num;               // defined in simulation.h
long long grid_size = 3;   // defined in simulation.h
long long leaderboard[20]; // defined in simulation.h

long long SIZE = 3;

void fillGrid(int size, bool even_gen, char new_state) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        writeCellState(i, j, k, even_gen, new_state);
      }
    }
  }
}

void checkGrid(int size, int value, int left, int right) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        assert(readCellState(i, j, k, false) == right);
        assert(readCellState(i, j, k, true) == left);
      }
    }
  }
}

void checkState(int size, bool even_gen, char expectedState) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        char state = even_gen ? readCellState(i, j, k, true)
                              : readCellState(i, j, k, false);
        char newState = calculateNextState(i, j, k, state != 0, even_gen);
        assert(expectedState == newState);
      }
    }
  }
}

void fillNcells(int size, int n, bool even_gen, char new_state) {
  fillGrid(size, even_gen, 0); // clear the grid
  int count = 0;
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      for (int k = 0; k < SIZE; k++) {
        if (count == n) {
          return;
        }
        writeCellState(i, j, k, even_gen, new_state);
        count++;
      }
    }
  }
}

void countGrid(int size, bool even_gen, int alive, int dead) {
  int count = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        char state = even_gen ? readCellState(i, j, k, true)
                              : readCellState(i, j, k, false);
        char newState = calculateNextState(i, j, k, state != 0, even_gen);
        if (newState == 1) {
          count++;
        }
      }
    }
  }
  assert(count == alive);
  assert(size * size * size - count == dead);
}

int main() {

  // all grids are initialized to 0
  grid = gen_initial_grid(SIZE, 0, 0);

  // All cells are dead
  fillGrid(SIZE, false, 0);
  checkState(SIZE, false, 0);

  // All cells are alive
  fillGrid(SIZE, false, 1);
  checkState(SIZE, false, 0);

  // a live cell with 0 to 4 live neighbors dies
  for (int i = 0; i < 6; i++) {
    fillNcells(SIZE, i, false, 1);
    checkState(SIZE, false, 0);
  }

  // 6 live cells
  fillNcells(SIZE, 6, false, 1);
  countGrid(SIZE, false, 6, SIZE * SIZE * SIZE - 6);

  // 7 to 10 live cells (will be alive in the next generation)
  for (int i = 7; i < 11; i++) {
    fillNcells(SIZE, i, false, 1);
    countGrid(SIZE, false, SIZE * SIZE * SIZE, 0);
  }

  // 11 to 13 live cells (nothing happens)
  for (int i = 11; i < 14; i++) {
    fillNcells(SIZE, i, false, 1);
    countGrid(SIZE, false, i, SIZE * SIZE * SIZE - i);
  }
  // 14 live cells
  fillNcells(SIZE, 14, false, 1);
  countGrid(SIZE, false, 14, SIZE * SIZE * SIZE - 14);

  // a live cell with more than 13 live neighbors dies
  for (int i = 15; i < SIZE * SIZE * SIZE; i++) {
    fillNcells(SIZE, i, false, 1);
    checkState(SIZE, false, 0);
  }

  return 0;
}