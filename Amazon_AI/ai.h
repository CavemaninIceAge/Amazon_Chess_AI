#ifndef AI_H
#define AI_H

#include "logic.h"

// Result of one computer move decision.
struct SearchResult {
    AmazonMove move;     // best move found; only meaningful when hasMove is true
    bool hasMove;        // false when the player has no legal move (game lost)
    int score;           // minimax value from the moving player's point of view
    int depthReached;    // deepest search iteration that completed in time
    long long nodes;     // positions visited
    int elapsedMs;       // wall-clock time spent
};

// Static evaluation of a position from `player`'s point of view:
// territory measured with queen-move and king-move distance maps, plus mobility.
int evaluatePosition(int board[SIZE][SIZE], int player);

// The original one-ply opponent: every legal move is scored by
// "own mobility - opponent mobility" and the best is played. Kept as a baseline.
SearchResult greedyMove(int board[SIZE][SIZE], int player);

// Minimax with alpha-beta pruning, iterative deepening within `timeLimitMs`,
// and a per-ply candidate limit (only the best-looking moves are searched deeper).
// `maxDepth` caps the number of plies; the search stops early when time runs out.
SearchResult searchBestMove(int board[SIZE][SIZE], int player, int maxDepth, int timeLimitMs);

#endif // AI_H
