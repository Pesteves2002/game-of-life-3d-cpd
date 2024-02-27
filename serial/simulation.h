#ifndef SIMULATION_H
#define SIMULATION_H

#include "leaderboard.h"
#include "utils.h"
#include <stdbool.h>

void initializeAux(Cube *c, int gen_num, int grid_size);

void simulation();

void debugPrintGrid();

void updateGridState();

void updateCellState(int i, int j, int k);

unsigned char readCellState(int index);

void writeCellState(int x, int y, int z, int index, unsigned char old_state,
                    unsigned char new_state);

void resetNeighborsCount(int x, int y, int z);

unsigned char calculateNextState(int x, int y, int z,
                                 unsigned char current_state, int index);
unsigned char getMostFrequentValue(int x, int y, int z);

#endif // SIMULATION_H
