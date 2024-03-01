#ifndef __LEADERBOARD_H__
#define __LEADERBOARD_H__

#include "utils.h"

void clearLeaderboard();

void updateMaxScores(int currentGen);

void writeToLeaderboard(long long *leaderboardToWrite);

void printLeaderboard();

#endif // __LEADERBOARD_H__
