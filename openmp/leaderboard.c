#include "leaderboard.h"

long long leaderboard[N_SPECIES * 3] = {0}; // current, max, max gen

void clearLeaderboard() {
  for (int i = 0; i < N_SPECIES; i++) {
    leaderboard[i] = 0;
  }
};

void updateMaxScores(int current_gen) {
  for (int i = 0; i < N_SPECIES; i++) {
    if (leaderboard[i] > leaderboard[i + N_SPECIES]) {
      leaderboard[i + N_SPECIES] = leaderboard[i];
      leaderboard[i + N_SPECIES * 2] = current_gen;
    }
  }
};

void writeToLeaderboard(char new_state) {
  if (new_state == 0) {
    return;
  }

#pragma omp atomic
  leaderboard[new_state - 1]++;
};

void printLeaderboard() {
  for (int i = 0; i < N_SPECIES; i++) {
    fprintf(stdout, "%d %lld %lld\n", i + 1, leaderboard[i + N_SPECIES],
            leaderboard[i + N_SPECIES * 2]);
  }
};
