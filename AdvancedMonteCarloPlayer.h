#ifndef ADVANCEDMONTECARLOPLAYER_H_
#define ADVANCEDMONTECARLOPLAYER_H_

#include <cstdlib>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <cmath>

using namespace std;

#include "Board.h"
#include "Player.h"

class AdvancedMonteCarloPlayer : public Player {
public:
    bool deterministicMode;
    int maxDepth;
    int simulationTimes;
    unordered_map<uint64_t, double> memo;

    AdvancedMonteCarloPlayer(bool d = true, int depth = 6, int times = 120) {
        deterministicMode = d;
        maxDepth = depth;
        simulationTimes = times;

        if (deterministicMode) {
            name = "Advanced Player on deterministic board";
        } else {
            name = "Advanced Player Player on non-deterministic board";
        }
    }

    double evaluateBoard(const Board& board) const {
        int n = board.getBoardSize();
        int maxTile = 0;
        int emptyCells = board.countEmptyCells();
        int cornerBonus = 0;
        int mergePotential = 0;
        int monotonicity = 0;
        int smoothness = 0;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int v = board.getGrid(i, j);

                if (v > maxTile) {
                    maxTile = v;
                }

                if (j + 1 < n && v != 0 && v == board.getGrid(i, j + 1)) {
                    mergePotential += v;
                }

                if (i + 1 < n && v != 0 && v == board.getGrid(i + 1, j)) {
                    mergePotential += v;
                }

                if (j + 1 < n) {
                    int right = board.getGrid(i, j + 1);
                    if (v >= right) {
                        monotonicity += 2;
                    }
                    smoothness -= abs(v - right);
                }

                if (i + 1 < n) {
                    int down = board.getGrid(i + 1, j);
                    if (v >= down) {
                        monotonicity += 2;
                    }
                    smoothness -= abs(v - down);
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
             + emptyCells * 250
             + cornerBonus * 6
             + mergePotential * 4
             + monotonicity * 12
             + smoothness;
    }

    uint64_t stateKey(const Board& board, int depthLeft) const {
        return board.hashValue() ^ (((uint64_t)depthLeft) << 56);
    }

    double deterministicSearch(Board board, int depthLeft) {
        if (board.hasWon()) {
            int totalScore = board.getCurrentScore() + board.getBonus();
            return 100000.0 + (double)totalScore;
        }

        if (!board.canMove()) {
            return board.getScoreperStep();
        }

        if (depthLeft == 0) {
            return evaluateBoard(board);
        }

        uint64_t key = stateKey(board, depthLeft);
        if (memo.find(key) != memo.end()) {
            return memo[key];
        }

        char possibleMoves[4] = {'l', 'r', 'u', 'd'};
        double bestValue = -1.0;

        for (char move : possibleMoves) {
            Board tempBoard = board;

            if (!tempBoard.makeMove(move)) {
                continue;
            }

            tempBoard.addNewTile();

            double value = deterministicSearch(tempBoard, depthLeft - 1);

            if (value > bestValue) {
                bestValue = value;
            }
        }

        if (bestValue < 0) {
            bestValue = evaluateBoard(board);
        }

        memo[key] = bestValue;
        return bestValue;
    }

    double rollout(Board board, int depthLeft) {
        if (board.hasWon()) {
            int totalScore = board.getCurrentScore() + board.getBonus();
            return 100000.0 + (double)totalScore;
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

        char randomMove = validMoves[rand() % validMoves.size()];
        board.makeMove(randomMove);
        board.addNewTile();

        return rollout(board, depthLeft - 1);
    }

    double simulation(Board board) {
        double total = 0.0;

        for (int i = 0; i < simulationTimes; i++) {
            Board tempBoard = board;
            total += rollout(tempBoard, maxDepth);
        }

        return total / simulationTimes;
    }

    char getMove(const Board& board) {
        memo.clear();

        priority_queue<Move> moves;
        char possibleMoves[4] = {'l', 'r', 'u', 'd'};

        for (char move : possibleMoves) {
            Board tempBoard = board;

            if (!tempBoard.makeMove(move)) {
                continue;
            }

            tempBoard.addNewTile();

            double utility;
            if (deterministicMode) {
                utility = deterministicSearch(tempBoard, maxDepth - 1);
            } else {
                utility = simulation(tempBoard);
            }

            cout << "Advanced score for move " << move
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