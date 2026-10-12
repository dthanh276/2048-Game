#ifndef GAME_H_
#define GAME_H_

#include "Board.h"
#include "Player.h"

class Game {
private:
    Board *board;
    Player *player;
    int NUMBEROFGAMES;
    bool deterministicMode;

    string moveStr(char move) {
        string res = "Unknown";
        if (move == 'l') res = "Left";
        if (move == 'r') res = "Right";
        if (move == 'u') res = "Up";
        if (move == 'd') res = "Down";
        return res;
    }

public:
    Game(int bs, Player *p, int n, bool d = false) {
        board = new Board(bs, d);
        player = p;
        NUMBEROFGAMES = n;
        deterministicMode = d;
    }

    virtual ~Game() {
        delete board;
    }

    void play();
};

void Game::play() {
    int won = 0;
    double averageScore = 0.0;
    int games = NUMBEROFGAMES;

    board->printBoard();

    while (games > 0) {
        if (board->hasWon()) {
            won++;

            cout << "Congratulations! You have reached the target number at step "
                 << board->getCurrentSteps() << "." << endl;

            cout << "Your current score is " << board->getCurrentScore() << "." << endl;

            int totalScore = board->getCurrentScore() + board->getBonus();
            double scorePerStep = (double)totalScore / (double)board->getCurrentSteps();

            cout << "Your final score per step is " << scorePerStep << endl;
            averageScore += scorePerStep;

            int bs = board->getBoardSize();
            delete board;
            board = new Board(bs, deterministicMode);

            games--;
            if (games > 0) board->printBoard();
            continue;
        }

        if (!board->canMove()) {
            cout << "Game Over at step " << board->getCurrentSteps()
                 << ". No more moves available.\n";
            cout << "Final Score per step: " << board->getScoreperStep() << "." << endl;

            averageScore += board->getScoreperStep();

            int bs = board->getBoardSize();
            delete board;
            board = new Board(bs, deterministicMode);

            games--;
            if (games > 0) board->printBoard();
            continue;
        }

        cout << "\nGame " << (NUMBEROFGAMES - games + 1)
             << " Step " << board->getCurrentSteps() + 1 << " ..." << endl;

        char move = player->getMove(*board);

        if (move == 'q') {
            cout << "Thanks for playing! Exiting game." << endl;
            break;
        }

        cout << player->getPlayerName() << " plays: " << moveStr(move)
             << " at step " << board->getCurrentSteps() << "." << endl;

        bool moved = board->makeMove(move);

        if (moved) {
            board->addNewTile();
            board->printBoard();

            Tile newTile = board->getNewTile();
            cout << "Added tile " << newTile.value << " at position ("
                 << newTile.x + 1 << ", " << newTile.y + 1 << ")." << endl;
        } else {
            cout << "Invalid move. Try again." << endl;
        }
    }

    if (NUMBEROFGAMES > 0) {
        averageScore /= (double)NUMBEROFGAMES;
    }

    cout << "Average score per step over " << NUMBEROFGAMES
         << " games: " << averageScore << endl;
    cout << "Total wins: " << won << " / " << NUMBEROFGAMES << endl;
}

#endif