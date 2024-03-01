#include "../simulation.h"
#include <assert.h>

const int SIZE = 3;
Cube *c;
int cPadding = SIZE + 2;

void fillGrid(int value) {
  for (int z = 1; z < cPadding - 1; z++) {
    for (int y = 1; y < cPadding - 1; y++) {
      for (int x = 1; x < cPadding - 1; x++) {
        int index = CALC_INDEX(x, y, z, cPadding);

        writeCellState(x, y, z, index, readCellState(index), value);
      }
    }
  }

  commitState();
}

void checkGrid(unsigned char value) {
  for (int z = 1; z < cPadding - 1; z++) {
    for (int y = 1; y < cPadding - 1; y++) {
      for (int x = 1; x < cPadding - 1; x++) {
        int index = CALC_INDEX(x, y, z, cPadding);

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

  fillGrid(0);
  checkGrid(0);

  // read left cell
  for (int i = 0; i < 16; i++) {
    fillGrid(i);
    checkGrid(i);
  }

  fillGrid(0);

  for (int i = 0; i < 16; i++) {
    fillGrid(i);
    checkGrid(i);
  }

  fprintf(stdout, "Successful test\n");

  return 0;
}
