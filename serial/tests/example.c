#include "../simulation.h"
#include <stdio.h>

int SIZE = 4;
int GENS = 4;

int main() {

  Cell ***g = gen_initial_grid(SIZE, 0.4, 100);

  simulation(&g, GENS, SIZE);

  fprintf(stdout, "Successful test\n");
  return 0;
}
