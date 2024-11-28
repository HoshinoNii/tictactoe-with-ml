#include <stdio.h>

// Function prototypes
void pvp(GameData *gameData, GameResources *resources);

// Main PvP game function
void pvp(GameData *gameData, GameResources *resources) {
  static bool gameStartSoundPlayed = false;
  static int restrictPlayer = false;

  // Play game start sound if not already played
  if (!gameStartSoundPlayed) {
    PlaySound(resources->gameStart);
    gameStartSoundPlayed = true;
  }

  // Display player labels
  DrawText("Player 1 [X]", 10, 20, 20, DARK_RED);
  DrawText("Player 2 [O]", GetScreenWidth() - 130, 20, 20, DARK_BLUE);

  if (!gameData->gameOver) {
    // Display game board
    display_board(gameData, resources);

    // Get player move
    getMove(gameData, resources->clickSound, &restrictPlayer);

    // Check for winner
    gameData->winner = check_winner(gameData->board);
    if (gameData->winner != EMPTY) {
      gameData->gameOver = true;
    }
  } else {
    declare_winner(gameData, &gameStartSoundPlayed, 0);
  }
}
