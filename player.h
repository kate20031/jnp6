#ifndef WORLDCUP_PLAYER_H
#define WORLDCUP_PLAYER_H

#include "board.h"
#include "exceptions.h"
#include <list>

class Player {
public:
    Player(std::string playerName, Board &board) :
        playerName(std::move(playerName)), money(initialMoney), status("w grze"), board(board), isAlive(true) {

        dies = board.getDies();
        squares = board.getSquares();
        squaresIt = squares.begin();
    }

    void play() {
        unsigned short score = 0;
        double change;

        // Count of dies is out of range
        if (dies.size() < 2) {
            throw TooFewDiceException();
        } else if (dies.size() > 2) {
            throw TooManyDiceException();
        }

        int waitingRoundsLeft = (*squaresIt)->tryLeave(playerName); // Player tries to start.

        if (waitingRoundsLeft != 0) { // Player has to wait.
            status = "*** czekanie: " + std::to_string(waitingRoundsLeft) + " ***";
            return;
        }

        for (const auto& die : dies) {
            score += die->roll();
        }

        printf("%s  wypadło %d\n", playerName.c_str(), score);

        for (int i = 0; i < score && isAlive; i++)
        {
            if (i == 0) {
                status = "w grze";
            }

            squaresIt++; // Player goes to the next square in the board.
            if (squaresIt == squares.end()) {
                squaresIt = squares.begin(); // Board is cyclic.
            }

            if (i == score - 1) {
                waitingRoundsLeft = (*squaresIt)->tryLeave(playerName);// Player tries to start.

                if (waitingRoundsLeft != 0) {
                    status = "*** czekanie: " + std::to_string(waitingRoundsLeft) + " ***";

                    return;
                }
            }

            if (i == score - 1) { // Player ends his round.
                change = (*squaresIt)->stayOn();
            }
            else {
                change = (*squaresIt)->goThrough();
            }

            if (change < 0 && money < (unsigned int)(-change)) { // Player went bankrupt :(
                while (i < score - 1) {
                    i++;
                    squaresIt++;
                    if (squaresIt == squares.end()) {
                        squaresIt = squares.begin(); // Board is cyclic.
                    }
                }
                isAlive = false;
                money = 0;
                status = "*** bankrut ***";
            }
            else {
                money += static_cast<unsigned int>(change);
            }
        }
    }

    std::string getName() {
        return playerName;
    }

    std::string getSquareName() {
        return (*squaresIt)->getName();
    }

    unsigned int getMoney() const {
        return money;
    }

    std::string getStatus() {
        return status;
    }

    bool getIsAlive() const {
        return isAlive;
    }

private:
    static const unsigned int initialMoney = 1000;
    std::string playerName;
    unsigned int money;
    std::string status;
    Board board;
    bool isAlive;
    std::list<std::shared_ptr<Square>>::iterator squaresIt;
    std::list<std::shared_ptr<Die>> dies;
    std::list<std::shared_ptr<Square>> squares;
};

#endif//WORLDCUP_PLAYER_H
