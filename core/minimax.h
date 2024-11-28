#define MM_NEG_INF -1000
#define MM_POS_INF 1000
#define MM_SCORE 20
#define MAX_DEPTH 5

double minimax(int board[MAX_FEATURES], int depth, bool isMax, double alpha,
               double beta);
// function prototypes (for game)
int check_winner(int board[MAX_FEATURES]);
bool isMovesLeft(int board[MAX_FEATURES]);
int getRandom(int min, int max);

void mmAI(GameData *gameData, GameResources *resources);

bool isMovesLeft(int board[MAX_FEATURES]) {
  for (int i = 0; i < MAX_FEATURES; i++) {
    if (board[i] == EMPTY) return true;
  }
  return false;
}

int getRandom(int min, int max) { return rand() % (max - min + 1) + min; }

unsigned long long int alphaBetaCalls = 0;

// Minimax Funtion, returns score based on who wins
double minimax(int board[MAX_FEATURES], int depth, bool isMax, double alpha,
               double beta) {
  alphaBetaCalls++;
  int score = check_winner(board);

  if (depth >= MAX_DEPTH) return 0;

  // If Maximizer has won the game
  if (score == O)
    return MM_SCORE - depth;  // subtracting score with depth offset to
                              // prioritize for faster wins

  // If Minimizer has won the game
  if (score == X)
    return -(MM_SCORE - depth);  // subtracting score with depth offset to
                                 // penalize slower losses

  // If there are no more moves it is a tie
  if (isMovesLeft(board) == false) return 0;

  // Maximizer's move
  if (isMax) {
    double best = MM_NEG_INF;

    // Traverse all cells
    for (int i = 0; i < MAX_FEATURES; i++) {
      // Check if cell is empty
      if (board[i] == EMPTY) {
        // Make the move
        board[i] = O;

        // Call minimax recursively and choose the maximum value
        best = fmax(best, minimax(board, depth + 1, false, alpha, beta));

        // Undo the move
        board[i] = EMPTY;

        // Alpha-Beta Pruning
        alpha = fmax(alpha, best);
        if (beta <= alpha) {
          break;
        }  // Beta cut-off to reduce redundant recursive searches
      }
    }
    return best;
  }

  // Minimizer's move
  else {
    double best = MM_POS_INF;

    // Traverse all cells
    for (int i = 0; i < MAX_FEATURES; i++) {
      // Check if cell is empty
      if (board[i] == EMPTY) {
        // Make the move
        board[i] = X;

        // Call minimax recursively and choose the minimum value
        best = fmin(best, minimax(board, depth + 1, true, alpha, beta));

        // Undo the move
        board[i] = EMPTY;

        // Alpha-Beta Pruning
        beta = fmin(beta, best);
        if (alpha >= beta) {
          break;
        }  // Alpha cut-off to reduce redundant recursive searches
      }
    }
    return best;
  }
}

void mmAI(GameData *gameData, GameResources *resources) {
  static bool gameStartSoundPlayed = false;
  static bool callOnce = true;
  static int restrictPlayer = false;

  if (!gameStartSoundPlayed) {
    PlaySound(resources->gameStart);
    gameStartSoundPlayed = true;
  }

  DrawText("Player 1 [X]", 10, 20, 20, DARK_RED);
  DrawText("Bot [O]", GetScreenWidth() - 130, 20, 20, DARK_BLUE);

  if (!gameData->gameOver) {
    display_board(gameData, resources);

    // player turn
    getMove(gameData, resources->clickSound, &restrictPlayer);

    // robot turn
    if (gameData->currentPlayer == O) {
      if (callOnce) {
        StartTimer(&ai_waitTimer, 0.5f);
        callOnce = false;
        restrictPlayer = true;
      }
      UpdateTimer(&ai_waitTimer);

      // wait for 0.5 seconds before making move
      DrawText("Bot is Thinking...", GetScreenWidth() / 2, 50, 20, DARK_BLUE);

      if (TimerDone(&ai_waitTimer)) {
        // AI move with imperfection
        double best_score = MM_NEG_INF, second_best_score = MM_NEG_INF;
        int best_move = -1, second_best_move = -1;
        for (int i = 0; i < MAX_FEATURES; i++) {
          if (gameData->board[i] == EMPTY) {
            gameData->board[i] = O;  // Simulate move
            double score = minimax(gameData->board, 0, false, MM_NEG_INF, MM_POS_INF);
            if (DEBUG) printf("Score for %d is %f\n", i, score);
            gameData->board[i] = EMPTY;  // Revert move

            if (score > best_score) {
              if (best_score != MM_NEG_INF) {
                second_best_score = best_score;
                second_best_move = best_move;
              }
              best_score = score;
              best_move = i;
            } else if (score > second_best_score) {
              second_best_score = score;
              second_best_move = i;
            }
          }
        }

        // AI picks a move, 30% chance of sub optimal move
        if (DEBUG) {
          printf("\nBest move is %d, Second best move is %d. ", best_move + 1,
                 second_best_move + 1);
        }

        if (best_score != MM_NEG_INF) {
          int selected_move = best_move;
          if (second_best_move != -1 && getRandom(1, 100) <= 30) {
            selected_move = second_best_move;
            if (DEBUG) printf("Second Best move selected");
          } else {
            if (DEBUG) printf("Best move selected");
          }
          gameData->board[selected_move] = O;
          if (DEBUG) printf("\nTotal Number of calls %llu", alphaBetaCalls);
        }
        callOnce = true;

        // return control to player after move
        restrictPlayer = false;
        gameData->currentPlayer = X;
      }
    }
    gameData->winner = check_winner(gameData->board);
    if (gameData->winner != EMPTY) {
      gameData->gameOver = true;
    }
  } else {
    restrictPlayer = false;
    declare_winner(gameData, &gameStartSoundPlayed, 1);
  }
}
