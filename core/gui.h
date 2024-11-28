#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function prototypes
void home_screen(GameState *gameState, Texture2D TTT_ICON);
void select_difficulty(GameState *gameState, Difficulty *selectedDifficulty);
static void draw_menu_button(int x, int y, int width, int height, const char *text);
static void handle_menu_clicks(GameState *gameState, int buttonX, int buttonY,
                               int buttonY2, int buttonWidth, int buttonHeight);
static void handle_difficulty_clicks(GameState *gameState, Difficulty *selectedDifficulty,
                                     int buttonX, int buttonY, int buttonY2,
                                     int buttonWidth, int buttonHeight,
                                     int backButtonX, int backButtonY,
                                     int backButtonWidth, int backButtonHeight);

void loadResources(GameResources *res);
void unloadResources(GameResources *res);

/**
 * Draws and handles interactions for the home screen
 * @param gameState Pointer to the current game state
 * @param TTT_ICON Texture containing the game icon
 */
void home_screen(GameState *gameState, Texture2D TTT_ICON) {
  // Calculate screen dimensions
  const int screenWidth = CELL_SIZE * GRID_SIZE;
  const int screenHeight = CELL_SIZE * GRID_SIZE + 100;

  // Icon setup and drawing
  const int iconScale = 5;
  const int iconWidth = TTT_ICON.width / iconScale;
  const int iconHeight = TTT_ICON.height / iconScale;
  const int iconX = 175;
  const int iconY = 60;

  // Draw scaled icon
  Rectangle source = {0, 0, TTT_ICON.width, TTT_ICON.height};
  Rectangle dest = {iconX, iconY, iconWidth, iconHeight};
  Vector2 origin = {0, 0};
  DrawTexturePro(TTT_ICON, source, dest, origin, 0.0f, WHITE);

  // Draw title
  const char *title = "Tic Tac Toe";
  DrawText(title, screenWidth / 2 - MeasureText(title, 30) / 2,
           screenHeight / 2 - 80, 30, OFF_WHITE);

  // Button setup
  const int buttonWidth = MeasureText("One Player", 20) + 20;
  const int buttonHeight = 40;
  const int buttonX = screenWidth / 2 - buttonWidth / 2;
  const int buttonY = screenHeight / 2;
  const int buttonSpacing = 20;
  const int buttonY2 = buttonY + buttonHeight + buttonSpacing;

  // Draw buttons
  draw_menu_button(buttonX, buttonY, buttonWidth, buttonHeight, "One Player");
  draw_menu_button(buttonX, buttonY2, buttonWidth, buttonHeight, "Two Player");

  // Handle button clicks
  handle_menu_clicks(gameState, buttonX, buttonY, buttonY2, buttonWidth, buttonHeight);
}

void loadResources(GameResources *res) {
  // Load all game audio and texture resources
  res->menuMusic = LoadSound("./resource/menumusic.wav");
  res->gameMusic = LoadSound("./resource/gamebeats.wav");
  res->gameStart = LoadSound("./resource/gamestart.wav");
  res->clickSound = LoadSound("./resource/gameclick.wav");
  res->cross = LoadTexture("./resource/x.png");
  res->circle = LoadTexture("./resource/o.png");
  res->icon = LoadTexture("./resource/tic-tac-toe.png");
}

void unloadResources(GameResources *res) {
  // Stop all sounds and unload resources to prevent memory leaks
  StopSound(res->menuMusic);
  StopSound(res->gameMusic);
  StopSound(res->clickSound);
  StopSound(res->gameStart);
  UnloadSound(res->menuMusic);
  UnloadSound(res->gameMusic);
  UnloadSound(res->clickSound);
  UnloadSound(res->gameStart);
  UnloadTexture(res->cross);
  UnloadTexture(res->circle);
  UnloadTexture(res->icon);
}

void handleAudio(GameState state, GameResources *res) {
  // Manage background music based on game state
  if (state == HOME) {
    if (!IsSoundPlaying(res->menuMusic)) PlaySound(res->menuMusic);
    StopSound(res->gameMusic);
  } else if (state == TWO_PLAYER || state == ONE_PLAYER) {
    if (!IsSoundPlaying(res->gameMusic)) PlaySound(res->gameMusic);
    StopSound(res->menuMusic);
  }
}

/**
 * Draws and handles interactions for the difficulty selection screen
 * @param gameState Pointer to the current game state
 * @param selectedDifficulty Pointer to the selected difficulty level
 */
void select_difficulty(GameState *gameState, Difficulty *selectedDifficulty) {
  // Screen setup
  const int screenWidth = CELL_SIZE * GRID_SIZE;
  const int screenHeight = CELL_SIZE * GRID_SIZE + 150;

  // Draw title
  const char *title = "Select Difficulty";
  DrawText(title, screenWidth / 2 - MeasureText(title, 30) / 2,
           screenHeight / 2 - 120, 30, OFF_WHITE);

  // Button dimensions
  const int buttonWidth = MeasureText("IMPOSSIBLE", 20) + 20;
  const int buttonHeight = 40;
  const int buttonSpacing = 20;
  const int buttonX = screenWidth / 2 - buttonWidth / 2;
  const int buttonY = screenHeight / 2 - 40;
  const int buttonY2 = buttonY + buttonHeight + buttonSpacing;

  // Draw difficulty buttons
  draw_menu_button(buttonX, buttonY, buttonWidth, buttonHeight, "Normal");
  draw_menu_button(buttonX, buttonY2, buttonWidth, buttonHeight, "Impossible");

  // Draw back button
  const int backButtonWidth = MeasureText("Back", 20) + 20;
  const int backButtonHeight = 40;
  const int backButtonX = screenWidth / 2 - backButtonWidth / 2;
  const int backButtonY = screenHeight - backButtonHeight - 30;
  draw_menu_button(backButtonX, backButtonY, backButtonWidth, backButtonHeight, "Back");

  // Handle button interactions
  handle_difficulty_clicks(gameState, selectedDifficulty, buttonX, buttonY, buttonY2,
                           buttonWidth, buttonHeight, backButtonX, backButtonY,
                           backButtonWidth, backButtonHeight);
}

/**
 * Helper function to draw a menu button
 */
static void draw_menu_button(int x, int y, int width, int height, const char *text) {
  DrawRectangle(x, y, width, height, OFF_WHITE);
  DrawText(text, x + 10, y + 10, 20, DARKGRAY);
}

/**
 * Helper function to handle menu button clicks
 */
static void handle_menu_clicks(GameState *gameState, int buttonX, int buttonY,
                               int buttonY2, int buttonWidth, int buttonHeight) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    if (mousePos.x >= buttonX && mousePos.x <= buttonX + buttonWidth) {
      if (mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
        *gameState = DIFFICULTY_SELECTION;
      } else if (mousePos.y >= buttonY2 && mousePos.y <= buttonY2 + buttonHeight) {
        *gameState = TWO_PLAYER;
      }
    }
  }
}

/**
 * Helper function to handle difficulty selection clicks
 */
static void handle_difficulty_clicks(GameState *gameState, Difficulty *selectedDifficulty,
                                     int buttonX, int buttonY, int buttonY2,
                                     int buttonWidth, int buttonHeight,
                                     int backButtonX, int backButtonY,
                                     int backButtonWidth, int backButtonHeight) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    if (mousePos.x >= buttonX && mousePos.x <= buttonX + buttonWidth) {
      if (mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
        *selectedDifficulty = NORMAL;
        *gameState = ONE_PLAYER;
      } else if (mousePos.y >= buttonY2 && mousePos.y <= buttonY2 + buttonHeight) {
        *selectedDifficulty = IMPOSSIBLE;
        *gameState = ONE_PLAYER;
      }
    }
    // Handle back button
    if (mousePos.x >= backButtonX && mousePos.x <= backButtonX + backButtonWidth &&
        mousePos.y >= backButtonY && mousePos.y <= backButtonY + backButtonHeight) {
      *gameState = HOME;
    }
  }
}
