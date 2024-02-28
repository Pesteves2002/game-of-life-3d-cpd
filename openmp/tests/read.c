#include "../simulation.h"
#include <assert.h>

int SIZE = 3;
Cube *c;

void fillGrid(int size, int value) {
  for (int z = 0; z < size; z++) {
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        int index = CALC_INDEX(x, y, z, size);

        writeCellState(x, y, z, index, readCellState(index), value);
      }
    }
  }

  commitState();
}

void checkGrid(int size, unsigned char value) {
  for (int z = 0; z < size; z++) {
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        int index = CALC_INDEX(x, y, z, size);

        assert(readCellState(index) == value);
      }
    }
  }
}

int main() {
  // all grids are initialized to 0
  c = gen_initial_grid(SIZE, 0, 0);
  initializeAux(c, 0, SIZE);

  simulation();

  fillGrid(SIZE, 0);
  checkGrid(SIZE, 0);

  // read left cell
  for (int i = 0; i < 16; i++) {
    fillGrid(SIZE, i);
    checkGrid(SIZE, i);
  }

  fillGrid(SIZE, 0);

  for (int i = 0; i < 16; i++) {
    fillGrid(SIZE, i);
    checkGrid(SIZE, i);
  }

  fprintf(stdout, "Successful test\n");

  return 0;
}
