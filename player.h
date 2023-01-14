#ifndef WORLDCUP_PLAYER_H
#define WORLDCUP_PLAYER_H

#include "board.h"
#include "exceptions.h"
#include <list>

class Player {
public:
    Player(std::string playerName, std::shared_ptr<Board> board) :
        playerName(std::move(playerName)), money(initialMoney), status("w grze"), board(board), isAlive(true), squaresIt(board->getBeginSquares()) {}

    void play() {
        unsigned short score;
        double change;

        int waitingRoundsLeft = (*squaresIt)->tryLeave(playerName); // Player tries to start.

        if (waitingRoundsLeft != 0) { // Player has to wait.
            status = "*** czekanie: " + std::to_string(waitingRoundsLeft) + " ***";
            return;
        }

        score = board->getRollScore();

        for (int i = 0; i < score && isAlive; i++)
        {
            if (i == 0) {
                status = "w grze";
            }

            board->setNextSquareIt(squaresIt);

            if (i == score - 1) { // The player ends his round.
                change = (*squaresIt)->stayOn();

                waitingRoundsLeft = (*squaresIt)->tryLeave(playerName); // Player tries to start (for the next round).
                if (waitingRoundsLeft != 0) {
                    status = "*** czekanie: " + std::to_string(waitingRoundsLeft) + " ***";
                    return;
                }

            }
            else {
                change = (*squaresIt)->goThrough();
            }

            if (change < 0 && money < (unsigned int)(-change)) { // Player went bankrupt :(
                while (i < score - 1) {
                    i++;
                    board->setNextSquareIt(squaresIt);
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

    std::string getName() const {
        return playerName;
    }

    std::string getSquareName() const {
        return (*squaresIt)->getName();
    }

    unsigned int getMoney() const {
        return money;
    }

    std::string getStatus() const {
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
    std::shared_ptr<Board> board;
    bool isAlive;
    std::list<std::shared_ptr<Square>>::const_iterator squaresIt;
    std::list<std::shared_ptr<Square>> squares;
};

#endif//WORLDCUP_PLAYER_H
