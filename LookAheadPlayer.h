#ifndef LOOKAHEADPLAYER_H_
#define LOOKAHEADPLAYER_H_

using namespace std;

#include "Board.h"
#include "Player.h"

class LookAheadPlayer : public Player {
public:
    int lookAheadDepth;

    LookAheadPlayer(int depth = 2) {
        name = "Look Ahead Player";
        lookAheadDepth = depth;
    }

    char getMove(const Board& board);

    int evaluateBoard(const Board& board) const {
        int n = board.getBoardSize();
        int maxTile = 0;
        int cornerBonus = 0;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board.getGrid(i, j) > maxTile) {
                    maxTile = board.getGrid(i, j);
                }
            }
        }

        if (board.getGrid(0, 0) == maxTile ||
            board.getGrid(0, n - 1) == maxTile ||
            board.getGrid(n - 1, 0) == maxTile ||
            board.getGrid(n - 1, n - 1) == maxTile) {
            cornerBonus = maxTile;
        }

        return board.getCurrentScore()
             + board.countEmptyCells() * 100
             + cornerBonus * 2;
    }

    int evaluateFuture(const Board& board, int depthLeft) const {
        if (depthLeft == 0 || !board.canMove()) {
            return evaluateBoard(board);
        }

        char possibleMoves[4] = {'l', 'r', 'u', 'd'};
        int bestValue = -1;

        for (char move : possibleMoves) {
            Board tempBoard = board;

            if (!tempBoard.makeMove(move)) {
                continue;
            }

            tempBoard.addNewTile();

            int value = evaluateFuture(tempBoard, depthLeft - 1);

            if (value > bestValue) {
                bestValue = value;
            }
        }

        if (bestValue == -1) {
            return evaluateBoard(board);
        }

        return bestValue;
    }
};

char LookAheadPlayer::getMove(const Board& board) {
    char possibleMoves[4] = {'l', 'r', 'u', 'd'};
    char bestMove = 'l';
    int bestValue = -1;

    for (char move : possibleMoves) {
        Board tempBoard = board;

        if (!tempBoard.makeMove(move)) {
            continue;
        }

        tempBoard.addNewTile();

        int value = evaluateFuture(tempBoard, lookAheadDepth - 1);

        if (value > bestValue) {
            bestValue = value;
            bestMove = move;
        }
    }

    return bestMove;
}

#endif 