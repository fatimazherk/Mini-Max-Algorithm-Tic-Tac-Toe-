#include "raylib.h"
#include <iostream>
#include <limits>

const int SIZE = 3;
char board[SIZE][SIZE];
bool playerTurn = true;
bool gameOver = false;
int winner = 0;
int roundCounter = 0;
float resetTimer = -1.0f; // Timer to track reset delay

void initializeBoard();
void drawBoard();
void handlePlayerInput(int mouseX, int mouseY);
void aiMove();
int minimax(int depth, bool isMax, int alpha, int beta);
int checkWin();
bool isMovesLeft();
void drawSidePanel();
void resetGame();

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Tic-Tac-Toe with AI");
    initializeBoard();

    while (!WindowShouldClose()) {
        // Check if it's time to reset the game
        if (gameOver && resetTimer > 0.0f && GetTime() > resetTimer) {
            resetGame();
        }

        // Game logic
        if (!gameOver) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && playerTurn) {
                handlePlayerInput(GetMouseX(), GetMouseY());
            }

            if (!playerTurn && !gameOver) {
                aiMove();
                playerTurn = true;
            }

            int result = checkWin();
            if (result != 0) {
                gameOver = true;
                winner = result;
                resetTimer = GetTime() + 3.0f; // Set a 3-second delay before resetting
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground((Color){245, 223, 228, 255}); // Pastel pink background
        drawBoard();
        drawSidePanel();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void initializeBoard() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

void drawBoard() {
    int cellSize = 200;
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();

    for (int i = 1; i < SIZE; i++) {
        DrawLine(i * cellSize, 0, i * cellSize, 600, (Color){198, 224, 205, 255}); // Pastel green grid
        DrawLine(0, i * cellSize, 600, i * cellSize, (Color){198, 224, 205, 255}); // Pastel green grid
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int x = j * cellSize;
            int y = i * cellSize;

            // Highlight cell on hover
            if (mouseX > x && mouseX < x + cellSize && mouseY > y && mouseY < y + cellSize && board[i][j] == ' ') {
                DrawRectangle(x, y, cellSize, cellSize, (Color){220, 240, 230, 100}); // Light green hover effect
            }

            int textX = x + cellSize / 2 - 30;
            int textY = y + cellSize / 2 - 70;

            if (board[i][j] == 'X') {
                DrawText("X", textX, textY, 100, (Color){243, 156, 160, 255}); // Pastel pink for player
            } else if (board[i][j] == 'O') {
                DrawText("O", textX, textY, 100, (Color){162, 217, 184, 255}); // Pastel green for AI
            }
        }
    }
}

void drawSidePanel() {
    const int panelWidth = 200;
    const int panelHeight = 600;
    const int xOffset = 600;

    DrawRectangle(xOffset, 0, panelWidth, panelHeight, (Color){240, 228, 236, 255}); // Light pastel pink panel
    DrawText("Player => X", xOffset + 10, 50, 30, (Color){243, 156, 160, 255});
    DrawText("AI => O", xOffset + 10, 100, 30, (Color){120, 180, 150, 255}); // Darker green for AI

    // Draw a cute bear aligned in the middle
    int bearX = xOffset + panelWidth / 2; // Center horizontally
    int bearY = panelHeight / 2;         // Center vertically

    // Bear's head
    DrawCircle(bearX, bearY, 40, (Color){160, 82, 45, 255}); // Brown color

    // Bear's ears
    DrawCircle(bearX - 30, bearY - 30, 20, (Color){160, 82, 45, 255}); // Left ear
    DrawCircle(bearX + 30, bearY - 30, 20, (Color){160, 82, 45, 255}); // Right ear

    // Inner ears
    DrawCircle(bearX - 30, bearY - 30, 10, (Color){222, 184, 135, 255}); // Left inner ear
    DrawCircle(bearX + 30, bearY - 30, 10, (Color){222, 184, 135, 255}); // Right inner ear

    // Bear's snout
    DrawCircle(bearX, bearY, 15, (Color){222, 184, 135, 255}); // Snout
    DrawCircle(bearX, bearY - 10, 5, BLACK);                   // Nose

    // Bear's face features based on the game outcome
    if (gameOver) {
        if (winner == 2) { // AI wins
            // Winking eye
            DrawLine(bearX - 18, bearY - 10, bearX - 12, bearY - 10, BLACK); // Left eye winking
            DrawCircle(bearX + 15, bearY - 10, 5, BLACK);                   // Right eye open

            // Thin smile
            DrawLine(bearX - 8, bearY + 15, bearX + 8, bearY + 15, BLACK);
        } else if (winner == 0) { // It's a draw
            // Sad eyes
            DrawCircle(bearX - 15, bearY - 10, 5, BLACK); // Left eye
            DrawCircle(bearX + 15, bearY - 10, 5, BLACK); // Right eye

            // Tear under left eye
            DrawCircle(bearX - 15, bearY, 3, (Color){135, 206, 250, 255}); // Small tear under the left eye (light blue)

            // Upside-down thin sad smile
            DrawLine(bearX - 8, bearY + 20, bearX + 8, bearY + 20, BLACK);  // Bottom line for the mouth
            DrawLine(bearX - 8, bearY + 20, bearX - 5, bearY + 18, BLACK); // Left curve of sad mouth
            DrawLine(bearX + 5, bearY + 18, bearX + 8, bearY + 20, BLACK); // Right curve of sad mouth
        } else { // Player wins
            // Happy eyes
            DrawCircle(bearX - 15, bearY - 10, 5, BLACK); // Left eye
            DrawCircle(bearX + 15, bearY - 10, 5, BLACK); // Right eye

            // Thin smile
            DrawLine(bearX - 8, bearY + 15, bearX + 8, bearY + 15, BLACK);
        }
    } else {
        // Neutral face
        DrawCircle(bearX - 15, bearY - 10, 5, BLACK); // Left eye
        DrawCircle(bearX + 15, bearY - 10, 5, BLACK); // Right eye
        DrawLine(bearX - 5, bearY + 10, bearX + 5, bearY + 10, BLACK); // Straight line for mouth
    }

    // Display the game result or instructions
    if (gameOver) {
        if (winner == 1) {
            DrawText("Result: Player Wins!", xOffset + 10, 150, 20, (Color){243, 156, 160, 255});
        } else if (winner == 2) {
            DrawText("Result: AI Wins!", xOffset + 10, 150, 20, (Color){120, 180, 150, 255}); // Darker green for AI
        } else {
            DrawText("Result: It's a Tie!", xOffset + 10, 150, 20, (Color){198, 224, 205, 255});
        }
    } else {
        DrawText("Make your move!", xOffset + 10, 150, 20, (Color){120, 180, 150, 255}); // Darker green for instructions
    }
}



void handlePlayerInput(int mouseX, int mouseY) {
    int cellSize = 200;
    int row = mouseY / cellSize;
    int col = mouseX / cellSize;

    if (row < SIZE && col < SIZE && board[row][col] == ' ') {
        board[row][col] = 'X';
        playerTurn = false;
    }
}

void aiMove() {
    int bestVal = -std::numeric_limits<int>::max();
    int bestRow = -1, bestCol = -1;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = 'O';
                int moveVal = minimax(0, false, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
                board[i][j] = ' ';
                if (moveVal > bestVal) {
                    bestRow = i;
                    bestCol = j;
                    bestVal = moveVal;
                }
            }
        }
    }

    board[bestRow][bestCol] = 'O';
}

int minimax(int depth, bool isMax, int alpha, int beta) {
    int score = checkWin();

    if (score == 2) return 10 - depth; // AI wins; prioritize faster wins
    if (score == 1) return depth - 10; // Player wins; penalize
    if (!isMovesLeft()) return 0;      // Tie

    if (isMax) {
        int best = -std::numeric_limits<int>::max();
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'O';
                    best = std::max(best, minimax(depth + 1, false, alpha, beta));
                    board[i][j] = ' ';
                    alpha = std::max(alpha, best);
                    if (beta <= alpha) break;
                }
            }
        }
        return best;
    } else {
        int best = std::numeric_limits<int>::max();
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'X';
                    best = std::min(best, minimax(depth + 1, true, alpha, beta));
                    board[i][j] = ' ';
                    beta = std::min(beta, best);
                    if (beta <= alpha) break;
                }
            }
        }
        return best;
    }
}

int checkWin() {
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') {
            return (board[i][0] == 'X') ? 1 : 2;
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') {
            return (board[0][i] == 'X') ? 1 : 2;
        }
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') {
        return (board[0][0] == 'X') ? 1 : 2;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') {
        return (board[0][2] == 'X') ? 1 : 2;
    }
    return 0;
}

bool isMovesLeft() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == ' ') return true;
        }
    }
    return false;
}

void resetGame() {
    initializeBoard();
    playerTurn = true;
    gameOver = false;
    winner = 0;
    resetTimer = -1.0f;
}
