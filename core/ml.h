#define MAX_FEATURES 9        // Number of features (board positions)
#define MAX_SAMPLES 1000      // Maximum number of training samples
#define LEARNING_RATE 0.01    // Learning rate for gradient descent
#define EPOCHS 1000          // Number of training iterations
#define MSE_THRESHOLD 0.05    // Maximum allowed mean squared error for imperfection
#define FORGETFULNESS 0.05     // Forgetfulness factor for AI player

// Common data structure for machine learning model
typedef struct {
  int features[MAX_SAMPLES][MAX_FEATURES];        // Input feature matrix
  int labels[MAX_SAMPLES];                        // Target labels
  double weights[MAX_FEATURES + 1];               // Model weights + bias term
  int sampleCount;                                // Number of samples loaded
  double trainSplit;                              // Train/test split ratio
  int trainSize;                                  // Size of training set
  int testSize;                                   // Size of test set

  // Training/testing data matrices
  int trainingFeatures[MAX_SAMPLES][MAX_FEATURES];  // Training features
  int testingFeatures[MAX_SAMPLES][MAX_FEATURES];   // Testing features
  int trainingLabels[MAX_SAMPLES];                  // Training labels
  int testingLabels[MAX_SAMPLES];                   // Testing labels

  // Model evaluation metrics
  int truePositives;      // Correctly predicted positive cases
  int trueNegatives;      // Correctly predicted negative cases
  int falsePositives;     // Incorrectly predicted positive cases
  int falseNegatives;     // Incorrectly predicted negative cases
  double precision;       // Precision metric
  double recall;          // Recall metric
  double f1Score;         // F1 score
  double errorRate;       // Classification error rate
} MLModel;

// Function prototypes
double calculate_error_probability(MLModel *model, int isTraining);
void calculate_confusion_matrix(MLModel *model, int isTraining);
void save_metrics_to_csv(MLModel *model);
void ml_init(MLModel *model);
void load_data(const char *filename, MLModel *model);
void shuffle_data(MLModel *model);
void train_linear_regression(MLModel *model);
void print_weights(double weights[MAX_FEATURES + 1]);
void print_gradients(double gradient[MAX_FEATURES + 1]);
double predict(int features[MAX_FEATURES], double weights[MAX_FEATURES + 1]);
double add_noise(double prediction);
int predict_move_with_imperfection(int features[], double weights[]);
void humanVsML(GameData *game, GameResources *res, double weights[MAX_FEATURES + 1]);

void evaluate_and_print_model_metrics(MLModel *model);

// Initialize and train the model
void ml_init(MLModel *model) {
  model->trainSplit = 0.8;  // Use 80% of data for training
  model->sampleCount = 0;   // Initialize sample counter

  // Load training data from file
  load_data("./core/dataset/tic-tac-toe.data", model);

  // Calculate sizes for train/test split
  model->trainSize = (int)(model->trainSplit * model->sampleCount);
  model->testSize = model->sampleCount - model->trainSize;

  // Split features and labels into training/testing sets
  for (int i = 0; i < model->trainSize; i++) {
    memcpy(model->trainingFeatures[i], model->features[i], sizeof(int) * MAX_FEATURES);
    model->trainingLabels[i] = model->labels[i];
  }
  for (int i = 0; i < model->testSize; i++) {
    memcpy(model->testingFeatures[i], model->features[model->trainSize + i], sizeof(int) * MAX_FEATURES);
    model->testingLabels[i] = model->labels[model->trainSize + i];
  }

  // Train the linear regression model
  printf("Training the Linear Regression Model...\n");
  train_linear_regression(model);

  // Print final weights and bias term
  printf("Trained Weights and Bias:\n");
  for (int i = 0; i < MAX_FEATURES; i++) {
    printf("Weight[%d]: %.4f\n", i, model->weights[i]);
  }
  printf("Bias: %.4f\n", model->weights[MAX_FEATURES]);

  evaluate_and_print_model_metrics(model);
}

void evaluate_and_print_model_metrics(MLModel *model) {
  // Calculate confusion matrices for training data
  calculate_confusion_matrix(model, 1);
  int TP_train = model->truePositives;
  int TN_train = model->trueNegatives;
  int FP_train = model->falsePositives;
  int FN_train = model->falseNegatives;

  // Calculate confusion matrices for testing data
  calculate_confusion_matrix(model, 0);
  int TP_test = model->truePositives;
  int TN_test = model->trueNegatives;
  int FP_test = model->falsePositives;
  int FN_test = model->falseNegatives;

  // Calculate training metrics
  double train_precision = (double)TP_train / (TP_train + FP_train + 1e-10);  // Add small constant to prevent div by 0
  double train_recall = (double)TP_train / (TP_train + FN_train + 1e-10);
  double train_f1 = 2 * (train_precision * train_recall) / (train_precision + train_recall + 1e-10);
  double train_error = calculate_error_probability(model, 1);

  // Calculate testing metrics
  double test_precision = (double)TP_test / (TP_test + FP_test + 1e-10);
  double test_recall = (double)TP_test / (TP_test + FN_test + 1e-10);
  double test_f1 = 2 * (test_precision * test_recall) / (test_precision + test_recall + 1e-10);
  double test_error = calculate_error_probability(model, 0);

  // Print training metrics
  printf("\nTraining Metrics:\n");
  printf("Precision: %.4f\n", train_precision);
  printf("Recall: %.4f\n", train_recall);
  printf("F1 Score: %.4f\n", train_f1);
  printf("Error Rate: %.4f\n", train_error);

  // Print testing metrics
  printf("\nTesting Metrics:\n");
  printf("Precision: %.4f\n", test_precision);
  printf("Recall: %.4f\n", test_recall);
  printf("F1 Score: %.4f\n", test_f1);
  printf("Error Rate: %.4f\n", test_error);

  // print confusion matrix data
  printf("\nConfusion Matrix Data:\n");
  printf("True Positives: %d\n", TP_test);
  printf("True Negatives: %d\n", TN_test);
  printf("False Positives: %d\n", FP_test);
  printf("False Negatives: %d\n", FN_test);


  // Store final model metrics
  model->precision = test_precision;
  model->recall = test_recall;
  model->f1Score = test_f1;
  model->errorRate = test_error;

  save_metrics_to_csv(model);
}

// Calculate classification error rate
double calculate_error_probability(MLModel *model, int isTraining) {
  int total_errors = 0;
  // Select appropriate dataset based on isTraining flag
  int *features = isTraining ? (int*)model->trainingFeatures : (int*)model->testingFeatures;
  int *labels = isTraining ? model->trainingLabels : model->testingLabels;
  int count = isTraining ? model->trainSize : model->testSize;

  // Count prediction errors
  for (int i = 0; i < count; i++) {
    double predicted = predict(&features[i * MAX_FEATURES], model->weights);
    if ((int)predicted != labels[i]) {
      total_errors++;
    }
  }

  return (double)total_errors / count;
}

// Calculate confusion matrix metrics
void calculate_confusion_matrix(MLModel *model, int isTraining) {
  // Reset metrics
  model->truePositives = 0;
  model->trueNegatives = 0;
  model->falsePositives = 0;
  model->falseNegatives = 0;

  // Select appropriate dataset
  int *features = isTraining ? (int*)model->trainingFeatures : (int*)model->testingFeatures;
  int *labels = isTraining ? model->trainingLabels : model->testingLabels;
  int count = isTraining ? model->trainSize : model->testSize;

  // Calculate metrics
  for (int i = 0; i < count; i++) {
    int predicted = (int)predict(&features[i * MAX_FEATURES], model->weights);
    int actual = labels[i];

    // Update appropriate counter based on prediction vs actual
    if (actual == 1) {
      if (predicted == 1)
        model->truePositives++;
      else
        model->falseNegatives++;
    } else {
      if (predicted == 1)
        model->falsePositives++;
      else
        model->trueNegatives++;
    }
  }
}

// Save model metrics to CSV file
void save_metrics_to_csv(MLModel *model) {
  FILE *file = fopen("metrics.csv", "w");
  if (file == NULL) {
    printf("Error opening file to save metrics.\n");
    return;
  }

  // Write headers and values
  fprintf(file, "Metric,Value\n");
  fprintf(file, "True Positive,%d\n", model->truePositives);
  fprintf(file, "True Negative,%d\n", model->trueNegatives);
  fprintf(file, "False Positive,%d\n", model->falsePositives);
  fprintf(file, "False Negative,%d\n", model->falseNegatives);
  fprintf(file, "Precision,%.6f\n", model->precision);
  fprintf(file, "Recall,%.6f\n", model->recall);
  fprintf(file, "F1-Score,%.6f\n", model->f1Score);

  fclose(file);
  printf("Metrics saved to 'metrics.csv'.\n");
}

// Load training data from file
void load_data(const char *filename, MLModel *model) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("Error opening file.\n");
    exit(1);
  }

  // Read file line by line
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    char *token = strtok(line, ",");
    int feature_index = 0;

    // Parse features
    while (token && feature_index < MAX_FEATURES) {
      if (token[0] == 'x')  // X moves
        model->features[model->sampleCount][feature_index++] = 1;
      else if (token[0] == 'o')  // O moves
        model->features[model->sampleCount][feature_index++] = -1;
      else  // Empty squares
        model->features[model->sampleCount][feature_index++] = 0;
      token = strtok(NULL, ",");
    }

    // Parse label
    model->labels[model->sampleCount] = (strcmp(token, "positive\n") == 0) ? 1 : 0;
    model->sampleCount++;
  }

  shuffle_data(model);
  fclose(file);
}

// Randomly shuffle the dataset
void shuffle_data(MLModel *model) {
  for (int i = model->sampleCount - 1; i > 0; i--) {
    int j = rand() % (i + 1);

    // Swap features
    for (int k = 0; k < MAX_FEATURES; k++) {
      int temp = model->features[i][k];
      model->features[i][k] = model->features[j][k];
      model->features[j][k] = temp;
    }

    // Swap labels
    int temp = model->labels[i];
    model->labels[i] = model->labels[j];
    model->labels[j] = temp;
  }
}

// Train linear regression model using gradient descent
void train_linear_regression(MLModel *model) {
  for (int epoch = 0; epoch < EPOCHS; epoch++) {
    double gradient[MAX_FEATURES + 1] = {0};  // Initialize gradients

    // Calculate gradients for all training examples
    for (int i = 0; i < model->trainSize; i++) {
      double predicted = model->weights[MAX_FEATURES];  // Bias term
      for (int j = 0; j < MAX_FEATURES; j++) {
        predicted += model->trainingFeatures[i][j] * model->weights[j];
      }

      double error = predicted - model->trainingLabels[i];

      // Update gradients
      for (int j = 0; j < MAX_FEATURES; j++) {
        gradient[j] += error * model->trainingFeatures[i][j];
      }
      gradient[MAX_FEATURES] += error;  // Bias gradient
    }

    // Update weights using gradient descent
    for (int j = 0; j <= MAX_FEATURES; j++) {
      model->weights[j] -= LEARNING_RATE * gradient[j] / model->trainSize;
    }
  }
}

// Make prediction for single example
double predict(int features[MAX_FEATURES], double weights[MAX_FEATURES + 1]) {
  double result = weights[MAX_FEATURES];  // Bias term
  for (int i = 0; i < MAX_FEATURES; i++) {
    result += weights[i] * features[i];
  }
  return result >= 0.5 ? 1 : 0;  // Binary classification threshold
}

// Add random noise to prediction
double add_noise(double prediction) {
  double noise = ((double)rand() / RAND_MAX) * 2 * MSE_THRESHOLD - MSE_THRESHOLD;  // Random noise within MSE range
  return prediction + noise;
}

// Make prediction with deliberate imperfection
int predict_move_with_imperfection(int features[], double weights[]) {
  double prediction = predict(features, weights);
  prediction = add_noise(prediction);  // Add noise
  printf("\nPrediction With Imperfection: %lf\n", prediction);
  return (prediction > 0.5) ? 1 : 0;
}

// Find best move for AI player
int get_best_ai_move(int board[MAX_FEATURES], double weights[MAX_FEATURES + 1]) {
  printf("\n\nGet Best Move");
  double best_score = DBL_MIN;
  int best_move = -1;

  // Try each possible move
  for (int i = 0; i < MAX_FEATURES; i++) {
    if (board[i] == EMPTY) {  // If square is empty

      if((double)rand() / RAND_MAX < FORGETFULNESS) {
        printf("\nAI Forgot, oh no! (Forgetful Factor) \n");
        continue; // Forgetfulness factor
      }

      board[i] = O;  // Try O move
      double score = predict_move_with_imperfection(board, weights);
      printf("Score[%d]: %lf\n", i, score);
      board[i] = EMPTY;  // Undo move

      // Update best move if better score found
      if (score > best_score) {
        best_score = score;
        best_move = i;
      }
    }
  }
  printf("The best move for the bot is: %d (row %d, col %d)\n", best_move, best_move / 3 + 1, best_move % 3 + 1);
  return best_move;
}

// Handle game play between human and ML model
void humanVsML(GameData *gameData, GameResources *resources, double weights[MAX_FEATURES + 1]) {
  static bool gameStartSoundPlayed = false;
  static bool callOnce = true;
  static int restrictPlayer = false;

  // Play start sound once
  if (!gameStartSoundPlayed) {
    PlaySound(resources->gameStart);
    gameStartSoundPlayed = true;
  }

  // Display player labels
  DrawText("Player 1 [X]", 10, 20, 20, DARK_RED);
  DrawText("Bot [O]", GetScreenWidth() - 130, 20, 20, DARK_BLUE);

  // Check for game over
  if (gameData->gameOver) {
    restrictPlayer = false;
    declare_winner(gameData, &gameStartSoundPlayed, 1);
    return;
  }

  display_board(gameData, resources);
  getMove(gameData, resources->clickSound, &restrictPlayer);

  // AI's turn
  if (gameData->currentPlayer == O) {
    if (callOnce) {
      StartTimer(&ai_waitTimer, 0.5f);  // Add delay for natural feel
      restrictPlayer = true;
      callOnce = false;
    }

    BeginDrawing();
    UpdateTimer(&ai_waitTimer);
    DrawText("Bot is Thinking...", GetScreenWidth() / 2, 50, 20, DARK_BLUE);

    // Make AI move after timer
    if (TimerDone(&ai_waitTimer)) {
      int best_move = get_best_ai_move(gameData->board, weights);
      if (best_move != -1) {
        gameData->board[best_move] = O;
      }
      EndDrawing();

      gameData->currentPlayer = X;
      restrictPlayer = false;
      callOnce = true;
    }
  }

  update_game_state(gameData, &gameStartSoundPlayed);
}
