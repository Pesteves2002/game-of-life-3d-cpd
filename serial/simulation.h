#ifndef SIMULATION_H
#define SIMULATION_H

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <map>

#include "leaderboard.h"

void simulation(char ****grid, int gen_num, int grid_size);

void debugPrintGrid(bool even_gen);

void updateGridState(bool even_gen);

char calculateNextState(int i, int j, int k, bool alive, bool even_gen);

void updateCellState(int i, int j, int k, bool even_gen);

char readCellState(int i, int j, int k, bool even_gen);

void writeCellState(int i, int j, int k, bool even_gen, char new_state);

char getMostFrequentValue(std::map<char, int> neighborsValues);

#endif // SIMULATION_H
