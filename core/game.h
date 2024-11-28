#include "../include/raylib.h"

#define MAX_FEATURES 9
#define DEBUG 1

#define MAX_FEATURES 9
#define CELL_SIZE 150  // Size of each grid
#define GRID_SIZE 3    // Number of grids

// Tic-Tac-Toe board constants
#define EMPTY 0
#define X 1
#define O -1
#define TIE 2

// Game states
typedef enum { HOME,
               TWO_PLAYER,
               DIFFICULTY_SELECTION,
               ONE_PLAYER } GameState;
typedef enum { NORMAL,
               IMPOSSIBLE } Difficulty;
typedef struct {
  float Lifetime;
} Timer;

// Structure to hold game resources
typedef struct {
  Sound menuMusic;
  Sound gameMusic;
  Sound gameStart;
  Sound clickSound;
  Texture2D cross;
  Texture2D circle;
  Texture2D icon;
} GameResources;

// Structure to hold game state
typedef struct {
  GameState state;
  Difficulty difficulty;
  int board[MAX_FEATURES];
  int currentPlayer;
  int winner;
  bool gameOver;
} GameData;

// Custom colors
Color BG_BLUE = (Color){30, 45, 65, 255};
Color DARK_BLUE = (Color){20, 180, 250, 255};
Color OFF_WHITE = (Color){250, 240, 230, 255};
Color DARK_RED = (Color){250, 30, 60, 255};

// function prototypes
void display_board(GameData *game, GameResources *resources);
extern int check_winner(int board[9]);  // assembly conversion
void getMove(GameData *game, Sound clickSound, int *restrictPlayer);
void declare_winner(GameData *game, bool *gameStartSoundPlayed, int isAI);
void drawAIStats(int screenWidth);
void resetGame(GameData *game, bool *gameStartSoundPlayed, int *callOnce);
void incrementAIWinCount();
void empty_board(int board[MAX_FEATURES]);
void StartTimer(Timer *timer, float lifetime);
void UpdateTimer(Timer *timer);
bool TimerDone(Timer *timer);
void initializeGame(GameData *game);
void update_game_state(GameData *gameData, bool *gameStartSoundPlayed);

// Global variables for AI win tracking
extern int ai_win_count;
extern int total_games;

// Contains Core functionality to the Tic-Tac-Toe game
// to be reused for multiple code files
int ai_win_count = 0;
int total_games = 0;
Timer ai_waitTimer = {0};

void initializeGame(GameData *game) {
  // Set initial game state and parameters
  game->state = HOME;
  game->difficulty = NORMAL;
  game->currentPlayer = X;
  game->winner = EMPTY;
  game->gameOver = false;
  empty_board(game->board);
}

void display_board(GameData *game, GameResources *resources) {
  const int PADDING = 35;         // Padding for X symbol
  const int CIRCLE_PADDING = 5;   // Padding for O symbol
  const int BOARD_OFFSET_Y = 50;  // Vertical offset for the board

  for (int row = 0; row < GRID_SIZE; row++) {
    for (int col = 0; col < GRID_SIZE; col++) {
      // Calculate positions
      int x = col * CELL_SIZE;
      int y = row * CELL_SIZE + BOARD_OFFSET_Y;
      int cell = row * GRID_SIZE + col;

      // Draw grid lines
      DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, OFF_WHITE);

      // Draw symbols based on cell state
      Rectangle destRect;
      switch (game->board[cell]) {
        case X:
          destRect = (Rectangle){
              x + PADDING,
              y + PADDING,
              CELL_SIZE - 2 * PADDING,
              CELL_SIZE - 2 * PADDING};
          DrawTexturePro(resources->cross,
                         (Rectangle){0, 0, resources->cross.width, resources->cross.height},
                         destRect, (Vector2){0, 0}, 0.0f, DARK_RED);
          break;

        case O:
          destRect = (Rectangle){
              x + CIRCLE_PADDING,
              y + CIRCLE_PADDING,
              CELL_SIZE - 2 * CIRCLE_PADDING,
              CELL_SIZE - 2 * CIRCLE_PADDING};
          DrawTexturePro(resources->circle,
                         (Rectangle){0, 0, resources->circle.width, resources->circle.height},
                         destRect, (Vector2){0, 0}, 0.0f, DARK_BLUE);
          break;
      }
    }
  }
}

// run this if environment is x64
#if defined(_WIN64)
// Function to check the winner
int check_winner(int board[MAX_FEATURES]) {
  // Define winning patterns as static const to ensure they're only created once
  static const int win_patterns[][3] = {
      {0, 1, 2}, {3, 4, 5}, {6, 7, 8},  // Rows
      {0, 3, 6},
      {1, 4, 7},
      {2, 5, 8},  // Columns
      {0, 4, 8},
      {2, 4, 6}  // Diagonals
  };
  static const int NUM_PATTERNS = 8;

  // Check all winning patterns
  for (int i = 0; i < NUM_PATTERNS; i++) {
    const int *pattern = win_patterns[i];
    if (board[pattern[0]] != EMPTY &&
        board[pattern[0]] == board[pattern[1]] &&
        board[pattern[1]] == board[pattern[2]]) {
      return board[pattern[0]];
    }
  }

  // Check for tie
  bool is_tie = true;
  for (int i = 0; i < MAX_FEATURES; i++) {
    if (board[i] == EMPTY) {
      is_tie = false;
      break;
    }
  }
  if (is_tie) return TIE;

  return EMPTY;  // Game is still ongoing
}
#endif

// function to increment AI count
void incrementAIWinCount() { ai_win_count++; }

// Helper functions for declare_winner
void updateGameStats(int winner, int isAI) {
  if (isAI && winner == O) {
    incrementAIWinCount();
  }
  total_games++;
}

void drawAIStats(int screenWidth) {
  double win_percentage = (total_games > 0) ? ((double)ai_win_count / total_games) * 100 : 0;

  DrawText(TextFormat("AI Win Count: %d", ai_win_count),
           screenWidth / 2 - MeasureText("AI Win Count: 0", 20) / 2,
           CELL_SIZE * GRID_SIZE - 10, 20, OFF_WHITE);

  DrawText(TextFormat("AI Win Percentage: %.2f%%", win_percentage),
           screenWidth / 2 - MeasureText("AI Win Percentage: 0.00%", 20) / 2,
           CELL_SIZE * GRID_SIZE - 40, 20, OFF_WHITE);
}

void resetGame(GameData *game, bool *gameStartSoundPlayed, int *callOnce) {
  empty_board(game->board);
  game->currentPlayer = X;
  game->gameOver = false;
  game->winner = EMPTY;
  *gameStartSoundPlayed = false;
  *callOnce = 1;
}

// Main declare_winner function
void declare_winner(GameData *game, bool *gameStartSoundPlayed, int isAI) {
  static int callOnce = 1;
  const int screenWidth = CELL_SIZE * GRID_SIZE;

  // Determine winner message
  const char *message = (game->winner == X) ? "Congratulations Player 1 (X), You Win!" : (game->winner == O) ? (isAI ? "AI (O) Wins!" : "Congratulations Player 2 (O), You Win!")
                                                                                                             : "It's a Tie!";

  if (callOnce) {
    updateGameStats(game->winner, isAI);
    callOnce = 0;
  }

  if (isAI) {
    drawAIStats(screenWidth);
  }

  // Display messages
  DrawText(message, screenWidth / 2 - MeasureText(message, 20) / 2,
           CELL_SIZE * GRID_SIZE + 20, 20, OFF_WHITE);
  DrawText("Press SPACE to play again",
           screenWidth / 2 - MeasureText("Press SPACE to play again", 20) / 2,
           CELL_SIZE * GRID_SIZE + 50, 20, OFF_WHITE);

  // Back to Menu button
  int buttonWidth = MeasureText("Back to Menu", 20) + 20;
  int buttonHeight = 40;
  int buttonX = screenWidth / 2 - buttonWidth / 2;
  int buttonY = CELL_SIZE * GRID_SIZE + 80;

  DrawRectangle(buttonX, buttonY, buttonWidth, buttonHeight, OFF_WHITE);
  DrawText("Back to Menu", buttonX + 10, buttonY + 10, 20, DARKGRAY);

  // Handle input
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    if (mousePos.x >= buttonX && mousePos.x <= buttonX + buttonWidth &&
        mousePos.y >= buttonY && mousePos.y <= buttonY + buttonHeight) {
      game->state = HOME;
      resetGame(game, gameStartSoundPlayed, &callOnce);
      return;
    }
  }

  if (IsKeyPressed(KEY_SPACE)) {
    resetGame(game, gameStartSoundPlayed, &callOnce);
  }
}

// Takes in game data and requests for move, and does checks for valid or invalid
void getMove(GameData *game, Sound clickSound, int *restrictPlayer) {
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && *restrictPlayer == false) {
    Vector2 mousePos = GetMousePosition();
    int col = mousePos.x / CELL_SIZE;
    int row = (mousePos.y - 50) / CELL_SIZE;
    int cell = row * GRID_SIZE + col;

    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE && game->board[cell] == EMPTY) {
      game->board[cell] = game->currentPlayer;
      game->currentPlayer = (game->currentPlayer == X) ? O : X;
      PlaySound(clickSound);
    }
  }
}

// Initialize empty board
void empty_board(int board[MAX_FEATURES]) {
  for (int i = 0; i < MAX_FEATURES; i++) board[i] = EMPTY;
}

// Simple Timer system for AI waits, instead of using sleep that may crash
// program start or restart a timer with a specific lifetime
void StartTimer(Timer *timer, float lifetime) {
  if (timer != NULL) timer->Lifetime = lifetime;
}

// update a timer with the current frame time
void UpdateTimer(Timer *timer) {
  // subtract this frame from the timer if it's not already expired
  if (timer != NULL && timer->Lifetime > 0) timer->Lifetime -= GetFrameTime();
}

// check if a timer is done.
bool TimerDone(Timer *timer) {
  if (timer != NULL) return timer->Lifetime <= 0;

  return false;
}

// Helper function to handle game state updates
void update_game_state(GameData *gameData, bool *gameStartSoundPlayed) {
  gameData->winner = check_winner(gameData->board);
  if (gameData->winner != EMPTY) {
    gameData->gameOver = true;
    declare_winner(gameData, gameStartSoundPlayed, 1);
  }
}
