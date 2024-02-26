#ifndef SIMULATION_H
#define SIMULATION_H

#include "leaderboard.h"
#include "utils.h"
#include <stdbool.h>

void simulation(Cube *c, int gen_num, int grid_size);

void debugPrintGrid(bool even_gen);

void updateGridState(bool even_gen);

void updateCellState(int i, int j, int k, bool even_gen);

unsigned char readCellState(int i, int j, int k, bool even_gen);

void writeCellState(int x, int y, int z, Cell *cell, bool even_gen,
                    unsigned char old_state, unsigned char new_state);

void resetNeighborsCount(int x, int y, int z);

unsigned char calculateNextState(int x, int y, int z,
                                 unsigned char current_state, int index,
                                 bool even_gen);

void getNeighborsValue(int x, int y, int z, bool even_gen,
                       unsigned char *neighborsValues);

unsigned char getMostFrequentValue(unsigned char *neighborsValues);

#endif // SIMULATION_H
