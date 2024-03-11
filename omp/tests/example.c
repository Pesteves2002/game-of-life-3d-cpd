#include "../simulation.h"

int SIZE = 4;
int GENS = 4;

int main() {

  Cube *cube = gen_initial_grid(SIZE, 0.4, 100);

  simulation(cube, GENS, SIZE);

  fprintf(stdout, "Successful test\n");
  return 0;
}
