#ifndef BOARD_H_
#define BOARD_H_

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <list>
#include <random>
#include <unordered_map>
#include <vector>

using namespace std;

static unordered_map<int, uint64_t> pieceKeys;
static int pieceKeyBoardSize = -1;
static const int MAX_ZOBRIST_TILE_INDEX = 20;

struct Tile {
    int x, y, value;
    Tile(int x = -1, int y = -1, int value = -1) : x(x), y(y), value(value) {}
};

class Board {
private:
    int boardSize;
    int **grid;
    int currentScore;
    int currentSteps;
    Tile newTile;
    int target = 2048;
    int bonus = 20000;

    mt19937_64 rng;
    bool deterministic;

    int tileToIndex(int value) const {
        if (value <= 0) {
            return 0;
        }

        int index = 0;
        while (value > 1) {
            value /= 2;
            index++;
        }

        if (index > MAX_ZOBRIST_TILE_INDEX) {
            index = MAX_ZOBRIST_TILE_INDEX;
        }

        return index;
    }

public:
    Board(int bs, bool d = false) {
        boardSize = bs;
        deterministic = d;

        if (deterministic) {
            rng.seed(1234);
        } else {
            random_device rd;
            rng.seed(rd());
        }

        initZobristHasher(boardSize);

        grid = new int *[boardSize];
        for (int i = 0; i < boardSize; i++) {
            grid[i] = new int[boardSize];
        }

        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                grid[i][j] = 0;
            }
        }

        currentScore = 0;
        currentSteps = 0;

        target = 2048 * ((int)((boardSize - 3) / 3) + 1);
        bonus = 10000 * ((int)(3 - (boardSize - 3) % 3));

        cout << "Based on the board size, the target number is " << target
             << " and the bonus score if winning is " << bonus << ".\n"
             << endl;

        addNewTile();
        addNewTile();
    }

    virtual ~Board() {
        for (int i = 0; i < boardSize; i++) {
            delete[] grid[i];
        }
        delete[] grid;
    }

    Board(const Board &cboard) {
        boardSize = cboard.getBoardSize();
        deterministic = cboard.deterministic;
        rng = cboard.rng;

        grid = new int *[boardSize];
        for (int i = 0; i < boardSize; i++) {
            grid[i] = new int[boardSize];
        }

        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                grid[i][j] = cboard.getGrid(i, j);
            }
        }

        currentScore = cboard.currentScore;
        currentSteps = cboard.currentSteps;
        target = cboard.target;
        bonus = cboard.bonus;
        newTile = cboard.newTile;
    }

    int getBoardSize() const { return boardSize; }
    int getGrid(int x, int y) const { return grid[x][y]; }
    int getTarget() const { return target; }
    int getBonus() const { return bonus; }
    int getCurrentSteps() const { return currentSteps; }
    int getCurrentScore() const { return currentScore; }
    bool isDeterministic() const { return deterministic; }

    double getScoreperStep() const {
        return currentSteps > 0 ? ((double)currentScore / (double)currentSteps) : 0;
    }

    Tile getNewTile() const { return newTile; }

    static void initZobristHasher(int bs) {
        if (pieceKeyBoardSize == bs) {
            return;
        }

        pieceKeys.clear();
        pieceKeyBoardSize = bs;

        random_device rd;
        mt19937_64 zobristRng(rd());
        uniform_int_distribution<uint64_t> dist;

        for (int i = 0; i < bs; ++i) {
            for (int j = 0; j < bs; ++j) {
                for (int tileIndex = 0; tileIndex <= MAX_ZOBRIST_TILE_INDEX; ++tileIndex) {
                    pieceKeys[tileIndex * bs * bs + i * bs + j] = dist(zobristRng);
                }
            }
        }
    }

    uint64_t hashValue() const {
        uint64_t h = 0;

        for (int i = 0; i < boardSize; ++i) {
            for (int j = 0; j < boardSize; ++j) {
                int tileIndex = tileToIndex(grid[i][j]);
                h ^= pieceKeys.at(tileIndex * boardSize * boardSize + i * boardSize + j);
            }
        }

        return h;
    }

    Tile generateNewTile() {
        vector<int> empty;

        for (int i = 0; i < boardSize; ++i) {
            for (int j = 0; j < boardSize; ++j) {
                if (grid[i][j] == 0) {
                    empty.push_back(i * boardSize + j);
                }
            }
        }

        if (empty.empty()) {
            return Tile(-1, -1, -1);
        }

        int idx = 0;
        int value = 0;

        if (deterministic) {
            uint64_t h = hashValue();
            idx = (int)(h % empty.size());
            value = (((h / empty.size()) % 10) == 0) ? 4 : 2;
        } else {
            uniform_int_distribution<int> valueDist(1, 10);
            uniform_int_distribution<int> indexDist(0, (int)empty.size() - 1);
            value = (valueDist(rng) == 10) ? 4 : 2;
            idx = indexDist(rng);
        }

        return Tile(empty[idx] / boardSize, empty[idx] % boardSize, value);
    }

    void addNewTile() {
        newTile = generateNewTile();
        if (newTile.x != -1) {
            grid[newTile.x][newTile.y] = newTile.value;
        }
    }

    bool hasWon() const {
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                if (grid[i][j] >= target) {
                    return true;
                }
            }
        }
        return false;
    }

    void printBoard() const {
        cout << "     ";
        for (int j = 0; j < boardSize; j++) {
            cout << j + 1 << "     ";
        }
        cout << endl;

        cout << "   ";
        for (int j = 0; j < boardSize; j++) {
            cout << "------";
        }
        cout << endl;

        for (int i = 0; i < boardSize; i++) {
            cout << setw(2) << i + 1 << " ";

            for (int j = 0; j < boardSize; j++) {
                if (grid[i][j] == 0) {
                    if (j == 0) cout << "|     |";
                    else cout << "     |";
                } else {
                    if (j == 0) cout << "|" << setw(5) << grid[i][j] << "|";
                    else cout << setw(5) << grid[i][j] << "|";
                }
            }
            cout << endl;

            cout << "   ";
            for (int j = 0; j < boardSize; j++) {
                cout << "------";
            }
            cout << endl;
        }

        cout << "Current score: " << currentScore
             << " Total steps: " << currentSteps << "." << endl;
        cout << "Current score per step: " << getScoreperStep() << "." << endl;
    }

    bool makeMove(char direction);
    vector<int> merge(list<int> row);
    int countEmptyCells() const;
    bool canMove() const;
};

int Board::countEmptyCells() const {
    int emptyCells = 0;
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if (grid[i][j] == 0) {
                emptyCells++;
            }
        }
    }
    return emptyCells;
}

bool Board::canMove() const {
    if (countEmptyCells() > 0) {
        return true;
    }

    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize - 1; j++) {
            if (grid[i][j] == grid[i][j + 1]) {
                return true;
            }
        }
    }

    for (int i = 0; i < boardSize - 1; i++) {
        for (int j = 0; j < boardSize; j++) {
            if (grid[i][j] == grid[i + 1][j]) {
                return true;
            }
        }
    }

    return false;
}

vector<int> Board::merge(list<int> row) {
    vector<int> mergedRow;

    auto i = row.begin();
    while (i != row.end()) {
        auto next = i;
        next++;

        if (next != row.end() && *i == *next) {
            int x = *i + *next;
            mergedRow.push_back(x);
            currentScore += x;
            i++;
            i++;
        } else {
            mergedRow.push_back(*i);
            i++;
        }
    }

    while ((int)mergedRow.size() < boardSize) {
        mergedRow.push_back(0);
    }

    return mergedRow;
}

bool Board::makeMove(char direction) {
    bool changed = false;

    for (int i = 0; i < boardSize; i++) {
        list<int> row;

        for (int j = 0; j < boardSize; j++) {
            if (direction == 'l' && grid[i][j] != 0) {
                row.push_back(grid[i][j]);
            } else if (direction == 'r' && grid[i][boardSize - 1 - j] != 0) {
                row.push_back(grid[i][boardSize - 1 - j]);
            } else if (direction == 'u' && grid[j][i] != 0) {
                row.push_back(grid[j][i]);
            } else if (direction == 'd' && grid[boardSize - 1 - j][i] != 0) {
                row.push_back(grid[boardSize - 1 - j][i]);
            }
        }

        vector<int> merged(boardSize, 0);
        if (!row.empty()) {
            merged = merge(row);
        }

        for (int j = 0; j < boardSize; j++) {
            if (direction == 'l') {
                if (grid[i][j] != merged[j]) changed = true;
                grid[i][j] = merged[j];
            } else if (direction == 'r') {
                if (grid[i][boardSize - 1 - j] != merged[j]) changed = true;
                grid[i][boardSize - 1 - j] = merged[j];
            } else if (direction == 'u') {
                if (grid[j][i] != merged[j]) changed = true;
                grid[j][i] = merged[j];
            } else if (direction == 'd') {
                if (grid[boardSize - 1 - j][i] != merged[j]) changed = true;
                grid[boardSize - 1 - j][i] = merged[j];
            }
        }
    }

    if (changed) {
        currentSteps++;
    }

    return changed;
}

#endif