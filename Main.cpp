#include <ctime>
#include <iostream>

using namespace std;

#include "Board.h"
#include "Game.h"
#include "HumanPlayer.h"
#include "Player.h"
#include "RandomPlayer.h"
#include "SmartPlayer.h"
#include "LookAheadPlayer.h"
#include "MonteCarloPlayer.h"
#include "AdvancedMonteCarloPlayer.h"

const int MAXDEPTH_DT = 5;
const int MAXDEPTH_MC = 2;
const int SIMULATION_TIMES = 30;

int main() {
    srand(time(0));

    int boardSize = 4;
    cout << "Input the size of board (>= 3):" << endl;
    cin >> boardSize;

    if (boardSize < 3)
        boardSize = 3;
    else if (boardSize > 15)
        boardSize = 15;

    cout << "Choose a player to play: " << endl;
    cout << "1. Human Player" << endl;
    cout << "2. Random Player" << endl;
    cout << "3. Smart Player" << endl;
    cout << "4. Smarter Player" << endl;
    cout << "5. Monte Carlo Player" << endl;
    cout << "6. Advanced Player" << endl;
    cout << "Others: Quit" << endl;

    int choice;
    cin >> choice;

    Player *p = nullptr;
    int games = 10;
    bool deterministic = false;

    if (choice == 1) {
        p = new HumanPlayer;
        games = 1;
    } else if (choice == 2) {
        p = new RandomPlayer;
    } else if (choice == 3) {
        p = new SmartPlayer;
    } else if (choice == 4) {
        p = new LookAheadPlayer(2);
    } else if (choice == 5) {
        p = new MonteCarloPlayer(MAXDEPTH_MC, SIMULATION_TIMES);
    } else if (choice == 6) {
        cout << "Should the game be deterministic? (Yes or No)?" << endl;
        string answer;
        cin >> answer;

        if (answer == "Yes" || answer == "yes" || answer == "Y" || answer == "y") {
            deterministic = true;
        }

        if (deterministic) {
            p = new AdvancedMonteCarloPlayer(true, MAXDEPTH_DT, SIMULATION_TIMES);
        } else {
            p = new AdvancedMonteCarloPlayer(false, MAXDEPTH_MC, SIMULATION_TIMES);
        }
    } else {
        return 0;
    }

    Game game(boardSize, p, games, deterministic);
    game.play();

    delete p;
    return 0;
}