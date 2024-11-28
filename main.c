#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Include custom header files for game functionality
#include "./core/game.h"
#include "./core/gui.h"
#include "./core/minimax.h"
#include "./core/ml.h"
#include "./core/multiplayer.h"
#include "./include/raylib.h"

// Function prototypes for main game functions
void initializeGame(GameData *game);
void loadResources(GameResources *resources);
void unloadResources(GameResources *resources);
void handleGameState(GameData *game, GameResources *resources, MLModel *model);
void handleGamePlay(GameData *game, GameResources *res, MLModel *model);

int main(void) {
  // Weights array for machine learning implementation
  MLModel model = {0};

// print computer env, x64 or arm
#if defined(_WIN64)
  printf("Environment: x64\n");
#elif defined(__aarch64__)
  printf("Environment: ARM\n");
#endif

  // Initialize machine learning weights
  ml_init(&model);

  // Set up window dimensions
  const int screenWidth = CELL_SIZE * GRID_SIZE;
  const int screenHeight = CELL_SIZE * GRID_SIZE + 130;

  // Initialize Raylib window and audio
  InitWindow(screenWidth, screenHeight, "TIC-TAC-TOE");
  InitAudioDevice();
  SetTargetFPS(60);

  // Create game resources and data structures
  GameResources resources;
  GameData gameData;

  // Initialize game resources and state
  loadResources(&resources);
  initializeGame(&gameData);

  // Main game loop
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BG_BLUE);
    handleGameState(&gameData, &resources, &model);
    EndDrawing();
  }

  // Cleanup and close
  unloadResources(&resources);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}

void handleGamePlay(GameData *game, GameResources *res, MLModel *model) {
  // Handle different game modes and difficulties
  if (game->state == ONE_PLAYER) {
    switch (game->difficulty) {
      case NORMAL:
        humanVsML(game, res, model->weights);  // ML-based AI for normal difficulty
        break;
      case IMPOSSIBLE:
        mmAI(game, res);  // Minimax AI for impossible difficulty
        break;
      default:
        game->state = HOME;
        break;
    }
  } else if (game->state == TWO_PLAYER) {
    pvp(game, res);  // Player vs Player mode
  }
}

void handleGameState(GameData *game, GameResources *res, MLModel *model) {
  // Manage audio and game states
  handleAudio(game->state, res);

  // Handle different game screens
  switch (game->state) {
    case HOME:
      home_screen(&game->state, res->icon);
      break;
    case DIFFICULTY_SELECTION:
      select_difficulty(&game->state, &game->difficulty);
      break;
    case TWO_PLAYER:
    case ONE_PLAYER:
      handleGamePlay(game, res, model);
      break;
  }
}
