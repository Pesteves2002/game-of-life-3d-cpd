#ifndef UTILS_H
#define UTILS_H

#define N_SPECIES (9)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char *gen_initial_grid(long long N, float density, int seed);

void writeBorders(unsigned char *grid, long long paddingSize, long long x,
                  long long y, long long z, unsigned char value);

#endif // UTILS_H
