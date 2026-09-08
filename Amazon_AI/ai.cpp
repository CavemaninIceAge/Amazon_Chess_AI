// Computer opponent for the Game of the Amazons.
//
// Search:   minimax with alpha-beta pruning, iterative deepening under a time
//           budget, and a per-ply candidate limit: at each node the legal moves
//           are ordered by a cheap one-ply mobility score and only the best few
//           are searched deeper (the branching factor is ~1000 in the opening,
//           so a full-width search beyond two plies is not affordable).
// Evaluate: territory from queen-move and king-move distance maps (the
//           "QueenMove / KingMove" idea from Lieberum, "An evaluation function
//           for the game of Amazons", 2005) plus a mobility term.
//
// Scores are always from the root player's point of view: the root player
// maximises, the opponent minimises.

#include "ai.h"

#include <algorithm>
#include <chrono>
#include <utility>
#include <vector>

namespace {

const int DR[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
const int DC[8] = {0, 0, -1, 1, -1, 1, -1, 1};
const int INF = 1000000;      // larger than any score
const int WIN = 100000;       // value of a won position (minus the ply it was found at)
const int UNREACHABLE = 99;   // distance for squares a player cannot reach
const int MAX_MOVES = 20000;  // getAllMoves() stops at 19999 moves

inline bool onBoard(int r, int c) { return r >= 0 && r < SIZE && c >= 0 && c < SIZE; }
inline int opponentOf(int p) { return p == WHITE ? BLACK : WHITE; }

// Apply a move in place; returns the piece that moved so the move can be undone.
inline int makeMove(int b[SIZE][SIZE], const AmazonMove& m) {
    int piece = b[m.startR][m.startC];
    b[m.startR][m.startC] = EMPTY;
    b[m.endR][m.endC] = piece;
    b[m.arrowR][m.arrowC] = ARROW;   // the arrow may land on the start square
    return piece;
}

inline void unmakeMove(int b[SIZE][SIZE], const AmazonMove& m, int piece) {
    b[m.arrowR][m.arrowC] = EMPTY;
    b[m.endR][m.endC] = EMPTY;
    b[m.startR][m.startC] = piece;
}

// Number of empty squares the player's amazons can move to (queen moves).
int mobility(int b[SIZE][SIZE], int player) {
    int total = 0;
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c) {
            if (b[r][c] != player) continue;
            for (int d = 0; d < 8; ++d) {
                int nr = r + DR[d], nc = c + DC[d];
                while (onBoard(nr, nc) && b[nr][nc] == EMPTY) {
                    ++total;
                    nr += DR[d];
                    nc += DC[d];
                }
            }
        }
    return total;
}

// Breadth-first search from all of `player`'s amazons. With kingMoves == false
// each step is a full queen move, otherwise a single step in one of 8 directions.
// dist[r][c] = minimum number of moves needed to reach the empty square (r, c).
void distanceMap(int b[SIZE][SIZE], int player, bool kingMoves, int dist[SIZE][SIZE]) {
    int queue[SIZE * SIZE];
    int head = 0, tail = 0;
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c) {
            dist[r][c] = UNREACHABLE;
            if (b[r][c] == player) {
                dist[r][c] = 0;
                queue[tail++] = r * SIZE + c;
            }
        }
    while (head < tail) {
        int cur = queue[head++];
        int r = cur / SIZE, c = cur % SIZE, next = dist[r][c] + 1;
        for (int d = 0; d < 8; ++d) {
            int nr = r + DR[d], nc = c + DC[d];
            while (onBoard(nr, nc) && b[nr][nc] == EMPTY) {
                if (dist[nr][nc] == UNREACHABLE) {   // first visit is the shortest (BFS)
                    dist[nr][nc] = next;
                    queue[tail++] = nr * SIZE + nc;
                }
                if (kingMoves) break;
                nr += DR[d];
                nc += DC[d];
            }
        }
    }
}

struct Searcher {
    int root = WHITE;
    long long nodes = 0;
    bool aborted = false;
    std::chrono::steady_clock::time_point deadline;
    std::vector<std::vector<AmazonMove>> moveBuf;             // one move buffer per ply
    std::vector<std::vector<std::pair<int, int>>> ordering;  // (key, index) per ply

    explicit Searcher(int maxPly)
        : moveBuf(maxPly + 1, std::vector<AmazonMove>(MAX_MOVES)), ordering(maxPly + 1) {}

    // Candidates searched at each ply; deeper plies keep fewer.
    static int candidateLimit(int ply) {
        static const int limits[] = {40, 24, 16, 12, 8, 8};
        return ply < 6 ? limits[ply] : 6;
    }

    bool outOfTime() {
        if (!aborted && std::chrono::steady_clock::now() >= deadline) aborted = true;
        return aborted;
    }

    int alphabeta(int b[SIZE][SIZE], int depth, int ply, int alpha, int beta, int player) {
        ++nodes;
        if (outOfTime()) return 0;   // the caller discards this iteration
        int opp = opponentOf(player);

        // A player who cannot move has lost. Prefer wins found at shallower plies.
        if (!hasAnyMove(b, player)) return player == root ? -(WIN - ply) : (WIN - ply);
        if (depth == 0) return evaluatePosition(b, root);

        AmazonMove* moves = moveBuf[ply].data();
        int n = getAllMoves(b, player, moves);

        // Order moves by a cheap one-ply score (mover's mobility minus the
        // opponent's) and keep only the best `limit` of them.
        std::vector<std::pair<int, int>>& order = ordering[ply];
        order.clear();
        for (int i = 0; i < n; ++i) {
            int piece = makeMove(b, moves[i]);
            int key = mobility(b, player) - mobility(b, opp);
            unmakeMove(b, moves[i], piece);
            order.push_back(std::make_pair(key, i));
        }
        int limit = std::min(n, candidateLimit(ply));
        std::partial_sort(order.begin(), order.begin() + limit, order.end(),
                          [](const std::pair<int, int>& a, const std::pair<int, int>& c) {
                              return a.first > c.first;
                          });

        bool maximising = (player == root);
        int best = maximising ? -INF : INF;
        for (int k = 0; k < limit; ++k) {
            const AmazonMove& m = moves[order[k].second];
            int piece = makeMove(b, m);
            int value = alphabeta(b, depth - 1, ply + 1, alpha, beta, opp);
            unmakeMove(b, m, piece);
            if (aborted) return 0;
            if (maximising) {
                if (value > best) best = value;
                if (best > alpha) alpha = best;
            } else {
                if (value < best) best = value;
                if (best < beta) beta = best;
            }
            if (alpha >= beta) break;   // the remaining moves cannot change the result
        }
        return best;
    }
};

}  // namespace

int evaluatePosition(int b[SIZE][SIZE], int player) {
    int opp = opponentOf(player);
    int qMe[SIZE][SIZE], qOp[SIZE][SIZE], kMe[SIZE][SIZE], kOp[SIZE][SIZE];
    distanceMap(b, player, false, qMe);
    distanceMap(b, opp, false, qOp);
    distanceMap(b, player, true, kMe);
    distanceMap(b, opp, true, kOp);

    // A square belongs to whoever reaches it first; contested squares count zero.
    int queenTerritory = 0, kingTerritory = 0;
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c) {
            if (b[r][c] != EMPTY) continue;
            if (qMe[r][c] < qOp[r][c]) ++queenTerritory;
            else if (qMe[r][c] > qOp[r][c]) --queenTerritory;
            if (kMe[r][c] < kOp[r][c]) ++kingTerritory;
            else if (kMe[r][c] > kOp[r][c]) --kingTerritory;
        }
    int mobilityDiff = mobility(b, player) - mobility(b, opp);
    return 10 * queenTerritory + 3 * kingTerritory + mobilityDiff;
}

SearchResult greedyMove(int board[SIZE][SIZE], int player) {
    SearchResult result;
    result.hasMove = false;
    result.score = 0;
    result.depthReached = 1;
    result.nodes = 0;
    result.elapsedMs = 0;

    std::vector<AmazonMove> moves(MAX_MOVES);
    int n = getAllMoves(board, player, moves.data());
    if (n == 0) return result;
    int opp = opponentOf(player);
    int best = -INF, bestIdx = 0;
    for (int i = 0; i < n; ++i) {
        int piece = makeMove(board, moves[i]);
        int score = mobility(board, player) - mobility(board, opp);
        unmakeMove(board, moves[i], piece);
        if (score > best) {
            best = score;
            bestIdx = i;
        }
    }
    result.hasMove = true;
    result.move = moves[bestIdx];
    result.score = best;
    result.nodes = n;
    return result;
}

SearchResult searchBestMove(int board[SIZE][SIZE], int player, int maxDepth, int timeLimitMs) {
    using Clock = std::chrono::steady_clock;
    Clock::time_point start = Clock::now();

    SearchResult result;
    result.hasMove = false;
    result.score = 0;
    result.depthReached = 0;
    result.nodes = 0;
    result.elapsedMs = 0;

    if (maxDepth < 1) maxDepth = 1;
    Searcher s(maxDepth + 1);
    s.root = player;
    s.deadline = start + std::chrono::milliseconds(timeLimitMs);

    std::vector<AmazonMove> rootMoves(MAX_MOVES);
    int n = getAllMoves(board, player, rootMoves.data());
    if (n == 0) return result;
    int opp = opponentOf(player);

    // Iteration 1: evaluate every legal move with the full evaluation. This is
    // both the depth-1 answer and the move ordering for the deeper iterations.
    std::vector<std::pair<int, int>> order;   // (score, index)
    order.reserve(n);
    for (int i = 0; i < n; ++i) {
        int piece = makeMove(board, rootMoves[i]);
        int score = hasAnyMove(board, opp) ? evaluatePosition(board, player) : WIN - 1;
        unmakeMove(board, rootMoves[i], piece);
        order.push_back(std::make_pair(score, i));
        ++s.nodes;
    }
    std::sort(order.begin(), order.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) { return a.first > b.first; });
    result.hasMove = true;
    result.move = rootMoves[order[0].second];
    result.score = order[0].first;
    result.depthReached = 1;

    // Iterations 2..maxDepth: alpha-beta over the best root candidates. An
    // iteration that runs out of time is discarded; the previous one stands.
    int limit = std::min(n, Searcher::candidateLimit(0));
    for (int depth = 2; depth <= maxDepth; ++depth) {
        if (result.score >= WIN - 1000 || result.score <= -(WIN - 1000)) break;   // outcome already forced
        int alpha = -INF, beta = INF;
        int bestScore = -INF, bestIdx = -1;
        for (int k = 0; k < limit; ++k) {
            const AmazonMove& m = rootMoves[order[k].second];
            int piece = makeMove(board, m);
            int value = s.alphabeta(board, depth - 1, 1, alpha, beta, opp);
            unmakeMove(board, m, piece);
            if (s.aborted) break;
            if (value > bestScore) {
                bestScore = value;
                bestIdx = order[k].second;
            }
            if (bestScore > alpha) alpha = bestScore;
        }
        if (s.aborted || bestIdx < 0) break;
        result.move = rootMoves[bestIdx];
        result.score = bestScore;
        result.depthReached = depth;
    }

    result.nodes = s.nodes;
    result.elapsedMs = (int)std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
    return result;
}
