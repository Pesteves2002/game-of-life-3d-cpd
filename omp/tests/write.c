#include "../simulation.h"
#include "../utils.h"

#include <assert.h>
#include <stdio.h>

int SIZE = 3;
Cube *c;

void fillGrid(int size, bool even_gen, char new_state) {
  // Reset the neighbors firts
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        if (even_gen) {
          GET_CELL(c, i, j, k).leftNeighbourCount = 0;
        } else {
          GET_CELL(c, i, j, k).rightNeighbourCount = 0;
        }
      }
    }
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        writeCellState(i, j, k, even_gen, readCellState(i, j, k, even_gen),
                       new_state);
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
  c = gen_initial_grid(SIZE, 0, 0);

  simulation(c, 0, SIZE);

  fillGrid(SIZE, false, 0);
  fillGrid(SIZE, true, 0);
  checkGrid(SIZE, 0, 0);

  for (int i = 0; i < 16; i++) {
    fillGrid(SIZE, false, i);
    checkGrid(SIZE, i, 0);
  }

  fillGrid(SIZE, false, 0);
  fillGrid(SIZE, true, 0);

  for (int i = 0; i < 16; i++) {
    fillGrid(SIZE, true, i);
    checkGrid(SIZE, 0, i);
  }

  fprintf(stdout, "Successful test\n");

  return 0;
}
