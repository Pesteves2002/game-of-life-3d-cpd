#include "leaderboard.h"

long long leaderboard[NUM_TYPE_ALIVE * 3] = {0}; // current, max, max gen

void clearLeaderboard() {
  for (int i = 0; i < NUM_TYPE_ALIVE; i++) {
    leaderboard[i] = 0;
  }
};

void updateMaxScores(int current_gen) {
  for (int i = 0; i < NUM_TYPE_ALIVE; i++) {
    if (leaderboard[i] > leaderboard[i + NUM_TYPE_ALIVE]) {
      leaderboard[i + NUM_TYPE_ALIVE] = leaderboard[i];
      leaderboard[i + NUM_TYPE_ALIVE * 2] = current_gen;
    }
  }
};

void writeToLeaderboard(char new_state) {
  if (new_state == 0) {
    return;
  }
  leaderboard[new_state - 1]++;
};

void printLeaderboard() {
  for (int i = 0; i < NUM_TYPE_ALIVE; i++) {
    std::cout << i + 1 << " " << leaderboard[i + NUM_TYPE_ALIVE] << " "
              << leaderboard[i + NUM_TYPE_ALIVE * 2] << std::endl;
  }
};
