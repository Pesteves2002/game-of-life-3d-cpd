#ifndef __LEADERBOARD_H__
#define __LEADERBOARD_H__

#include "utils.h"

void initLeaderboard();

void clearLeaderboard();

void updateMaxScores(int currentGen);

void writeToLeaderboard(unsigned char state);

void printLeaderboard();

#endif // __LEADERBOARD_H__
