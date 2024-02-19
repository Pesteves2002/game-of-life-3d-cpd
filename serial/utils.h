#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <cstdlib>

#define NUM_TYPE_ALIVE (9)

struct Cell {
  unsigned char leftState : 4;
  unsigned char rightState : 4;
};

Cell ***gen_initial_grid(long long cells_per_side, float density, int seed);

#endif // UTILS_H
