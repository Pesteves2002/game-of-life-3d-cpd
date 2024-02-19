#include "../simulation.h"
#include "../utils.h"

#include <bitset>
#include <cassert>

int SIZE = 4;
int GENS = 4;

int main() {

  char ***g = gen_initial_grid(SIZE, 0.4, 100);

  simulation(&g, GENS, SIZE);

  std::cout << "Successful test" << std::endl;
  return 0;
}
