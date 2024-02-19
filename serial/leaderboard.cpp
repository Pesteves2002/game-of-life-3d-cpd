#include "leaderboard.h"

long long leaderboard[NUM_TYPE_ALIVE * 3] = {0};

void initLeaderboard() {
  for (int i = 0; i < 9; i++) {
    leaderboard[i + 9] = leaderboard[i];
    leaderboard[i + 18] = 0;
  }
};

void clearLeaderboard() {
  for (int i = 0; i < 9; i++) {
    leaderboard[i] = 0;
  }
};

void updateMaxScores(int current_gen) {
  for (int i = 0; i < 9; i++) {
    if (leaderboard[i] > leaderboard[i + 9]) {
      leaderboard[i + 9] = leaderboard[i];
      leaderboard[i + 18] = current_gen;
    }
  }
};

void writeToLeaderboard(int new_state) {
  if (new_state == 0) {
    return;
  }
  leaderboard[new_state - 1]++;
};

void printLeaderboard() {
  for (int i = 0; i < 9; i++) {
    std::cout << i + 1 << " " << leaderboard[i + 9] << " "
              << leaderboard[i + 18] << std::endl;
  }
};
