// Self-play check for the Amazons AI (no Qt needed).
//
// Build:  clang++ -O2 -std=c++17 -I Amazon_AI tests/selfplay.cpp Amazon_AI/ai.cpp Amazon_AI/logic.cpp -o selfplay
// Run:    ./selfplay [games] [ms-per-move]
//
// 1. make/unmake round trip leaves the board unchanged;
// 2. the search finds a one-move trap (arrow that leaves the opponent no move);
// 3. minimax + alpha-beta plays the old one-ply greedy opponent from randomised
//    openings, alternating colours, and the score is printed.

#include "ai.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <vector>

static int opponentOf(int p) { return p == WHITE ? BLACK : WHITE; }

static void initialBoard(int b[SIZE][SIZE]) {
    memset(b, 0, sizeof(int) * SIZE * SIZE);
    b[0][3] = BLACK; b[0][6] = BLACK; b[3][0] = BLACK; b[3][9] = BLACK;
    b[6][0] = WHITE; b[6][9] = WHITE; b[9][3] = WHITE; b[9][6] = WHITE;
}

static void apply(int b[SIZE][SIZE], const AmazonMove& m) {
    int piece = b[m.startR][m.startC];
    b[m.startR][m.startC] = EMPTY;
    b[m.endR][m.endC] = piece;
    b[m.arrowR][m.arrowC] = ARROW;
}

static bool sameBoard(int a[SIZE][SIZE], int b[SIZE][SIZE]) {
    return memcmp(a, b, sizeof(int) * SIZE * SIZE) == 0;
}

static int failures = 0;
#define CHECK(cond, msg) do { if (!(cond)) { ++failures; printf("FAIL: %s\n", msg); } else printf("ok:   %s\n", msg); } while (0)

// Plays one game. `searchSide` uses searchBestMove, the other side greedyMove.
// Returns the winner (WHITE or BLACK).
static int playGame(int searchSide, unsigned seed, int timeMs, long long& nodes, int& searchMoves, int& depthSum, int& plies) {
    int b[SIZE][SIZE];
    initialBoard(b);
    std::mt19937 rng(seed);
    std::vector<AmazonMove> buf(20000);
    int player = WHITE;
    for (int i = 0; i < 2; ++i) {   // two random plies so that games differ
        int n = getAllMoves(b, player, buf.data());
        apply(b, buf[rng() % n]);
        player = opponentOf(player);
    }
    plies = 2;
    while (true) {
        if (!hasAnyMove(b, player)) return opponentOf(player);
        SearchResult r = (player == searchSide) ? searchBestMove(b, player, 6, timeMs) : greedyMove(b, player);
        if (player == searchSide) { nodes += r.nodes; depthSum += r.depthReached; ++searchMoves; }
        apply(b, r.move);
        player = opponentOf(player);
        if (++plies > 400) return 0;   // cannot happen: every move removes a square
    }
}

int main(int argc, char** argv) {
    int games = argc > 1 ? atoi(argv[1]) : 4;
    int timeMs = argc > 2 ? atoi(argv[2]) : 200;

    // 1. make/unmake: searching must leave the board exactly as it was.
    {
        int b[SIZE][SIZE], copy[SIZE][SIZE];
        initialBoard(b);
        memcpy(copy, b, sizeof(b));
        SearchResult r = searchBestMove(b, WHITE, 3, 300);
        CHECK(r.hasMove && sameBoard(b, copy), "search leaves the board unchanged");
        CHECK(r.depthReached >= 2, "reaches at least depth 2 in the opening within 300 ms");
        printf("      opening: depth %d, %lld nodes, %d ms\n", r.depthReached, r.nodes, r.elapsedMs);
    }

    // 2. One-move trap: black's only amazon at (0,0) has (1,1) as its last exit.
    {
        int b[SIZE][SIZE];
        memset(b, 0, sizeof(b));
        b[0][0] = BLACK; b[0][1] = ARROW; b[1][0] = ARROW;
        b[5][5] = WHITE;
        SearchResult r = searchBestMove(b, WHITE, 4, 300);
        CHECK(r.hasMove && r.move.arrowR == 1 && r.move.arrowC == 1, "finds the arrow that leaves black without a move");
        CHECK(r.score > 90000, "scores the trap as a forced win");
    }

    // 3. Self-play against the one-ply greedy baseline.
    int searchWins = 0;
    long long nodes = 0;
    int searchMoves = 0, depthSum = 0;
    for (int g = 0; g < games; ++g) {
        int searchSide = (g % 2 == 0) ? WHITE : BLACK;
        int plies = 0;
        int winner = playGame(searchSide, 1000u + g, timeMs, nodes, searchMoves, depthSum, plies);
        bool won = (winner == searchSide);
        searchWins += won;
        printf("game %d: minimax plays %s, %d plies, %s\n", g + 1, searchSide == WHITE ? "white" : "black", plies, won ? "minimax wins" : "greedy wins");
    }
    printf("minimax+alpha-beta vs greedy: %d-%d over %d games (%d ms/move), avg depth %.2f, avg %lld nodes/move\n",
           searchWins, games - searchWins, games, timeMs,
           searchMoves ? (double)depthSum / searchMoves : 0.0,
           searchMoves ? nodes / searchMoves : 0LL);
    CHECK(searchWins * 2 > games, "minimax wins the majority of games against greedy");

    printf("%s\n", failures == 0 ? "ALL CHECKS PASSED" : "SOME CHECKS FAILED");
    return failures == 0 ? 0 : 1;
}
