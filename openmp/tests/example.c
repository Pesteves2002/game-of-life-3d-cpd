#include "../simulation.h"

const int SIZE = 4;
int GENS = 4;

int main() {

  Cube *cube = gen_initial_grid(SIZE, 0.4f, 100);

  initializeAux(cube, GENS, SIZE);
  simulation();

  fprintf(stdout, "Successful test\n");
  return 0;
}
