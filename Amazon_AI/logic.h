#ifndef LOGIC_H
#define LOGIC_H

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE 10
#define EMPTY 0
#define WHITE 1
#define BLACK 2
#define ARROW 3

typedef struct {
    int startR, startC;
    int endR, endC;
    int arrowR, arrowC;
    int player;
} AmazonMove;

typedef struct {
    int x, y;
} Pos;
int isValidMove(int b[SIZE][SIZE], Pos start, Pos end);
int getAllMoves(int board[10][10], int player, AmazonMove moves[]);
int hasAnyMove(int board[10][10], int player);
#ifdef __cplusplus
}
#endif

#endif // LOGIC_H
