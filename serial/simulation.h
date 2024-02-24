#ifndef SIMULATION_H
#define SIMULATION_H

#include "leaderboard.h"
#include "utils.h"
#include <stdbool.h>

void simulation(Cube *c, int gen_num, int grid_size);

void debugPrintGrid(bool even_gen);

void updateNeighborsCount(int x, int y, int z, unsigned char value);

void updateGridState(bool even_gen);

unsigned char calculateNextState(int i, int j, int k, bool alive,
                                 bool even_gen);

void updateCellState(int i, int j, int k, bool even_gen);

unsigned char readCellState(int i, int j, int k, bool even_gen);

void writeCellState(int i, int j, int k, bool even_gen,
                    unsigned char old_state, unsigned char new_state);

void resetNeighborsCount(int x, int y, int z);

void getNeighborsValue(int x, int y, int z, bool even_gen, unsigned char *neighborsValues);

unsigned char getMostFrequentValue(unsigned char *neighborsValues);

#endif // SIMULATION_H
