#include "../simulation.h"
#include "../utils.h"

#include <bitset>
#include <cassert>

char ***grid;            // defined in simulation.h
int gen_num;             // defined in simulation.h
long long grid_size = 4; // defined in simulation.h

long long SIZE = 4;

int main() {

  grid = gen_initial_grid(SIZE, 0.4, 100);

  simulation();

  std::cout << "Successful test" << std::endl;
  return 0;
}
