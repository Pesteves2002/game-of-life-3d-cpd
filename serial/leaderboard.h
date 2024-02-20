#ifndef __LEADERBOARD_H__
#define __LEADERBOARD_H__

#include "utils.h"
#include <stdio.h>

void initLeaderboard();

void clearLeaderboard();

void updateMaxScores(int currentGen);

void writeToLeaderboard(char state);

void printLeaderboard();

#endif // __LEADERBOARD_H__
