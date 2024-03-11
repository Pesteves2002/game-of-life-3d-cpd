#include "leaderboard.h"

long long leaderboard[(N_SPECIES + 1) * 3] = {0}; // current, max, max gen

void updateMaxScores(int current_gen) {
  for (int i = 1; i < N_SPECIES + 1; i++) {
    if (leaderboard[i] > leaderboard[i + N_SPECIES]) {
      leaderboard[i + N_SPECIES] = leaderboard[i];
      leaderboard[i + N_SPECIES * 2] = current_gen;
    }
    leaderboard[i] = 0;
  }
};

void writeToLeaderboard(long long leaderboardToWrite[N_SPECIES + 1]) {
  for (int i = 0; i < N_SPECIES + 1; i++) {
#pragma omp atomic
    leaderboard[i] += leaderboardToWrite[i];
  }
};

void printLeaderboard() {
  for (int i = 1; i < N_SPECIES + 1; i++) {
    fprintf(stdout, "%d %lld %lld\n", i, leaderboard[i + N_SPECIES],
            leaderboard[i + N_SPECIES * 2]);
  }
};