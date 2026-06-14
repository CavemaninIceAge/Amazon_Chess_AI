#include "logic.h"
#include <stdlib.h>
int isValidMove(int b[SIZE][SIZE], Pos start, Pos end) {
    if (end.x < 0 || end.x >= SIZE || end.y < 0 || end.y >= SIZE) return 0;
    if (b[end.x][end.y] != EMPTY) return 0;

    int dx = end.x - start.x;
    int dy = end.y - start.y;

    if (dx != 0 && dy != 0 && abs(dx) != abs(dy)) return 0;

    int stepX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
    int stepY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

    int curX = start.x + stepX;
    int curY = start.y + stepY;

    while (curX != end.x || curY != end.y) {
        if (b[curX][curY] != EMPTY) return 0;
        curX += stepX;
        curY += stepY;
    }
    return 1;
}

int getAllMoves(int board[10][10], int player, AmazonMove moves[]) {
    int count = 0;
    int dr[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dc[] = {0, 0, -1, 1, -1, 1, -1, 1};
    for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
            if (board[r][c] == player) {
                for (int d1 = 0; d1 < 8; d1++) {
                    for (int step1 = 1; step1 < 10; step1++) {
                        int nr = r + dr[d1]*step1, nc = c + dc[d1]*step1;
                        if (nr<0 || nr>=10 || nc<0 || nc>=10 || board[nr][nc] != 0) break;
                        for (int d2 = 0; d2 < 8; d2++) {
                            for (int step2 = 1; step2 < 10; step2++) {
                                int ar = nr + dr[d2]*step2, ac = nc + dc[d2]*step2;
                                if (ar<0 || ar>=10 || ac<0 || ac>=10) break;
                                if (board[ar][ac] != 0 && !(ar == r && ac == c)) break;
                                moves[count].startR = r;
                                moves[count].startC = c;
                                moves[count].endR = nr;
                                moves[count].endC = nc;
                                moves[count].arrowR = ar;
                                moves[count].arrowC = ac;
                                moves[count].player = player;
                                count++;
                                if (count >= 19999) return count;
                            }
                        }
                    }
                }
            }
        }
    }
    return count;
}
int hasAnyMove(int board[10][10], int player) {
    int dr[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dc[] = {0, 0, -1, 1, -1, 1, -1, 1};
    for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
            if (board[r][c] == player) {
                for (int d = 0; d < 8; d++) {
                    int nr = r + dr[d], nc = c + dc[d];
                    if (nr >= 0 && nr < 10 && nc >= 0 && nc < 10 && board[nr][nc] == 0)
                        return 1;
                }
            }
        }
    }
    return 0;
}
