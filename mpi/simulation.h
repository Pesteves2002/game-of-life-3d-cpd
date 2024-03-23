#ifndef SIMULATION_H
#define SIMULATION_H

#include "utils.h"
#include <string.h>

void initializeAux(unsigned char *c, int gen_num, long long grid_size, int me,
                   int num_procs, int axis[1], MPI_Comm comm);

void simulation();

void debugPrintGrid();

void debugPrintNeighbors();

void updateGridState();

unsigned char updateCellState(long long x, long long y, long long z,
                              long long index);

unsigned char readCellState(long long index);

void writeCellState(long long x, long long y, long long z, long long index,
                    unsigned char old_state, unsigned char new_state);

void resetNeighborsCount(long long x, long long y, long long z);

unsigned char calculateNextState(long long x, long long y, long long z,
                                 unsigned char current_state, long long index);

unsigned char getNeighbourCount(long long x, long long y, long long z);

unsigned char getMostFrequentValue(long long x, long long y, long long z);

void updateMaxScores(int gen_num);

void printLeaderboard();

#endif // SIMULATION_H
