#ifndef UTILS_H
#define UTILS_H

#define N_SPECIES (9)

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char leftState : 4;
  unsigned char rightState : 4;
} Cell;

Cell ***gen_initial_grid(long long cells_per_side, float density, int seed);

#endif // UTILS_H
