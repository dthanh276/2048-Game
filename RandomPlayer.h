#ifndef RANDOMPLAYER_H_
#define RANDOMPLAYER_H_

using namespace std;

#include <cstdlib>
#include <vector>
#include "Board.h"
#include "Player.h"

class RandomPlayer : public Player {
public:
    RandomPlayer() {
        name = "Random Player";
    }

    char getMove(const Board &board);
};

char RandomPlayer::getMove(const Board &board) {
    vector<char> validMoves;
    char possibleMoves[4] = {'l', 'r', 'u', 'd'};

    for (char move : possibleMoves) {
        Board tempBoard = board;
        if (tempBoard.makeMove(move)) {
            validMoves.push_back(move);
        }
    }

    if (validMoves.empty()) {
        return 'l';
    }

    int index = rand() % validMoves.size();
    return validMoves[index];
}

#endif