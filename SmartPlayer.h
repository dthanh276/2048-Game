#ifndef SMARTPLAYER_H_
#define SMARTPLAYER_H_

using namespace std;

#include "Board.h"
#include "Player.h"

class SmartPlayer : public Player {
public:
    SmartPlayer() {
        name = "Smart Player";
    }

    char getMove(const Board &board);
};

char SmartPlayer::getMove(const Board &board) {
    char possibleMoves[4] = {'l', 'r', 'u', 'd'};
    char bestMove = 'l';
    int bestValue = -1;
    int scoreBefore = board.getCurrentScore();

    for (char move : possibleMoves) {
        Board tempBoard = board;

        if (!tempBoard.makeMove(move)) {
            continue;
        }

        int scoreGained = tempBoard.getCurrentScore() - scoreBefore;
        int n = tempBoard.getBoardSize();
        int maxTile = 0;
        int cornerBonus = 0;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (tempBoard.getGrid(i, j) > maxTile) {
                    maxTile = tempBoard.getGrid(i, j);
                }
            }
        }

        if (tempBoard.getGrid(0, 0) == maxTile ||
            tempBoard.getGrid(0, n - 1) == maxTile ||
            tempBoard.getGrid(n - 1, 0) == maxTile ||
            tempBoard.getGrid(n - 1, n - 1) == maxTile) {
            cornerBonus = maxTile;
        }

        int value = scoreGained + tempBoard.countEmptyCells() * 100 + cornerBonus * 2;

        if (value > bestValue) {
            bestValue = value;
            bestMove = move;
        }
    }

    return bestMove;
}

#endif