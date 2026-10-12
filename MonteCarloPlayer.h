#ifndef MONTECARLOPLAYER_H_
#define MONTECARLOPLAYER_H_

#include <cstdlib>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

#include "Board.h"
#include "Player.h"

class MonteCarloPlayer : public Player {
public:
    int maxDepth;
    int simulationTimes;

    MonteCarloPlayer(int depth = 4, int times = 100) {
        name = "Monte Carlo Player";
        maxDepth = depth;
        simulationTimes = times;
    }

    double evaluateBoard(const Board& board) const {
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

    double expansion(Board board, int depthLeft) {
        if (board.hasWon()) {
            int totalScore = board.getCurrentScore() + board.getBonus();
            if (board.getCurrentSteps() > 0) {
                return (double) totalScore / board.getCurrentSteps();
            }
            return (double) totalScore;
        }

        if (!board.canMove()) {
            return board.getScoreperStep();
        }

        if (depthLeft == 0) {
            return evaluateBoard(board);
        }

        vector<char> validMoves;
        char possibleMoves[4] = {'l', 'r', 'u', 'd'};

        for (char move : possibleMoves) {
            Board tempBoard = board;
            if (tempBoard.makeMove(move)) {
                validMoves.push_back(move);
            }
        }

        if (validMoves.empty()) {
            return board.getScoreperStep();
        }

        char randomMove;
        if (validMoves.size() == 1) {
            randomMove = validMoves[0];
        } else {
            randomMove = validMoves[rand() % validMoves.size()];
        }

        board.makeMove(randomMove);
        board.addNewTile();

        return expansion(board, depthLeft - 1);
    }

    double simulation(Board board) {
        double total = 0.0;

        for (int i = 0; i < simulationTimes; i++) {
            Board tempBoard = board;
            total += expansion(tempBoard, maxDepth);
        }

        return total / simulationTimes;
    }

    char getMove(const Board& board) {
        priority_queue<Move> moves;
        char possibleMoves[4] = {'l', 'r', 'u', 'd'};

        for (char move : possibleMoves) {
            Board tempBoard = board;

            if (!tempBoard.makeMove(move)) {
                continue;
            }

            tempBoard.addNewTile();

            if (tempBoard.hasWon()) {
                cout << "Simulation score for move " << move
                     << " = immediate win" << endl;
                return move;
            }

            double utility = simulation(tempBoard);

            cout << "Simulation score for move " << move
                 << " = " << utility << endl;

            moves.push(Move(move, utility));
        }

        if (!moves.empty()) {
            return moves.top().direction;
        }

        return 'l';
    }
};

#endif