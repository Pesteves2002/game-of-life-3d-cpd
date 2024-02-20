#include "../simulation.h"
#include <assert.h>
#include <stdio.h>

int SIZE = 3;
Cell ***g;

void fillGrid(int size, int value, bool even) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        if (even) {
          g[i][j][k].leftState = value;
        } else {
          g[i][j][k].rightState = value;
        }
      }
    }
  }
}

void checkGrid(int size, int left, int right) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        assert(readCellState(i, j, k, false) == right);
        assert(readCellState(i, j, k, true) == left);
      }
    }
  }
}

int main() {

  // all grids are initialized to 0
  g = gen_initial_grid(SIZE, 0, 0);

  simulation(&g, 0, SIZE);

  fillGrid(SIZE, 0, false);
  fillGrid(SIZE, 0, true);
  checkGrid(SIZE, 0, 0);

  // read left cell
  for (int i = 0; i < 16; i++) {
    fillGrid(SIZE, i, false);
    checkGrid(SIZE, 0, i);
  }

  fillGrid(SIZE, 0, false);
  fillGrid(SIZE, 0, true);

  for (int i = 0; i < 16; i++) {
    fillGrid(SIZE, i, true);
    checkGrid(SIZE, i, 0);
  }

  fprintf(stdout, "Successful test\n");

  return 0;
}
