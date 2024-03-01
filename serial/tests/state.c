#include "../simulation.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int SIZE = 3;
int PADDING_SIZE = 5;
Cube *c;

void fillGrid(int size, int value) {
  memset(c->grid, value, size * size * size);
  memset(c->neighbourCount, value == 0 ? 0 : 26, size * size * size);

  commitState();
}

void checkState(int size, char expectedState) {
  for (int z = 0; z < size; z++) {
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        int index = CALC_INDEX(x, y, z, size);

        unsigned char currentState = readCellState(index);
        unsigned char newState =
            calculateNextState(x, y, z, currentState, index);

        assert(expectedState == newState);
      }
    }
  }
}

void fillNcells(int size, int n, char new_state) {
  int count = 0;

  for (int z = 1; z < size - 1; z++) {
    for (int y = 1; y < size - 1; y++) {
      for (int x = 1; x < size - 1; x++) {
        int index = CALC_INDEX(x, y, z, size);
        if (count == n) {
          commitState();
          return;
        }

        writeCellState(x, y, z, index, readCellState(index), new_state);
        count++;
      }
    }
  }
}

void countGrid(int size, int alive, int dead) {
  int count = 0;
  int total = 0;
  for (int z = 1; z < size - 1; z++) {
    for (int y = 1; y < size - 1; y++) {
      for (int x = 1; x < size - 1; x++) {
        int index = z * size * size + y * size + x;

        unsigned char state = readCellState(index);
        unsigned char newState = calculateNextState(x, y, z, state, index);
        if (newState != 0) {
          count++;
        }
        total++;
      }
    }
  }

  assert(count == alive);
  size -= 2;
  assert(size * size * size - count == dead);
}

int main() {
  // all grids are initialized to 0
  c = gen_initial_grid(SIZE, 0, 0);
  initializeAux(c, 0, SIZE);

  simulation();

  // All cells are dead
  fillGrid(PADDING_SIZE, 0);
  checkState(SIZE, 0);

  // All cells are alive
  fillGrid(PADDING_SIZE, 1);
  checkState(SIZE, 0);

  // a live cell with 0 to 4 live neighbors dies
  for (int i = 0; i < 6; i++) {
    fillGrid(PADDING_SIZE, 0);
    fillNcells(PADDING_SIZE, i, 1);
    checkState(PADDING_SIZE, 0);
  }

  // 6 live cells
  fillGrid(PADDING_SIZE, 0);
  fillNcells(PADDING_SIZE, 6, 1);
  countGrid(PADDING_SIZE, 6, SIZE * SIZE * SIZE - 6);

  // 7 to 10 live cells (will be alive in the next generation)
  for (int i = 7; i < 11; i++) {
    fillGrid(PADDING_SIZE, 0);
    fillNcells(PADDING_SIZE, i, 1);
    countGrid(PADDING_SIZE, SIZE * SIZE * SIZE, 0);
  }

  // 11 to 13 live cells (nothing happens)
  for (int i = 11; i < 14; i++) {
    fillGrid(PADDING_SIZE, 0);
    fillNcells(PADDING_SIZE, i, 1);
    countGrid(PADDING_SIZE, i, SIZE * SIZE * SIZE - i);
  }

  // 14 live cells
  fillGrid(PADDING_SIZE, 0);
  fillNcells(PADDING_SIZE, 14, 1);
  countGrid(PADDING_SIZE, 14, SIZE * SIZE * SIZE - 14);

  // a live cell with more than 13 live neighbors dies
  for (int i = 15; i < SIZE * SIZE * SIZE; i++) {
    fillGrid(PADDING_SIZE, 0);
    fillNcells(PADDING_SIZE, i, 1);
    checkState(PADDING_SIZE, 0);
  }

  fprintf(stdout, "Successful test\n");

  return 0;
}
