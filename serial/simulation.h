#ifndef SIMULATION_H
#define SIMULATION_H

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <map>

extern char ***grid;

extern int gen_num;

extern long long grid_size;

extern long long leaderboard[20]; // contains the last generation and the
                                  // maximum population of each run
void simulation();

void debugPrintGrid(bool even_gen);

void initializeLeaderboard();

void clearCurrentLeaderboard();

void updateMaxLeaderboard();

void printLeaderboard();

void writeToLeaderboard(char new_state);

void updateGridState(bool even_gen);

char calculateNextState(int i, int j, int k, bool alive, bool even_gen);

void updateCellState(int i, int j, int k, bool even_gen);

char readCellState(int i, int j, int k, bool even_gen);

void writeCellState(int i, int j, int k, bool even_gen, char new_state);

char getMostFrequentValue(std::map<char, int> neighborsValues);

#endif // SIMULATION_H
