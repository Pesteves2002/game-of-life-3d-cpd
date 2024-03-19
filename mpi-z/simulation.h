#ifndef SIMULATION_H
#define SIMULATION_H

#include "utils.h"
#include <mpi.h>
#include <stdbool.h>
#include <string.h>

void initializeAux(unsigned char *c, int gen_num, int grid_size, int me,
                   int num_procs, int axis[3], MPI_Comm comm);

void simulation();

void debugPrintGrid();

void debugPrintNeighbors();

void updateGridState();

unsigned char updateCellState(int x, int y, int z, int index);

unsigned char readCellState(int index);

void writeCellState(int x, int y, int z, int index, unsigned char old_state,
                    unsigned char new_state);

void resetNeighborsCount(int x, int y, int z);

unsigned char calculateNextState(int x, int y, int z,
                                 unsigned char current_state, int index);

unsigned char getNeighbourCount(int x, int y, int z);

unsigned char getMostFrequentValue(int x, int y, int z);

void updateMaxScores(int gen_num);

void printLeaderboard();

#endif // SIMULATION_H
