#include "../simulation.h"
#include "../utils.h"

#include <bitset>
#include <cassert>

int SIZE = 3;
char ***g;

void fillGrid(int size, bool even_gen, char new_state) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      for (int k = 0; k < size; k++) {
        writeCellState(i, j, k, even_gen, new_state);
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

  checkGrid(SIZE, 0, 0);

  for (int i = 0; i < 16; i++) {
    fillGrid(SIZE, false, i);
    checkGrid(SIZE, i, 0);
  }

  fillGrid(SIZE, false, 0);

  for (int i = 0; i < 16; i++) {
    fillGrid(SIZE, true, i);
    checkGrid(SIZE, 0, i);
  }

  std::cout << "Successful test" << std::endl;

  return 0;
}
