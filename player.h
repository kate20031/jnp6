#ifndef WORLDCUP_PLAYER_H
#define WORLDCUP_PLAYER_H

#include "board.h"
#include "exceptions.h"
#include <list>

class Player {
public:
    Player(std::string playerName, std::shared_ptr<Board> board) :
        playerName(std::move(playerName)), money(initialMoney), status("w grze"), board(board), 
            isAlive(true), squaresIt(board->getBeginSquares()) {}

    void play() {
        unsigned short score;
        double change;

        int waitingRoundsLeft = (*squaresIt)->tryLeave(playerName); // The player tries to start.

        if (waitingRoundsLeft != 0) { // The player has to wait.
            status = "*** czekanie: " + std::to_string(waitingRoundsLeft) + " ***";
            return;
        }

        score = board->getRollScore();

        //std::cerr << "Gracz: " << playerName << " wyrzucil: " << score << std::endl;

        if (score == 0) { // The player again stays on its current sqare.
            change = (*squaresIt)->stayOn(money);
            if (change < 0 && money < (unsigned int)(-change)) // The player went bankrupt :(
                setBankrupt();
            else
                money += static_cast<unsigned int>(change);
        }

        for (int i = 0; i < score && isAlive; i++)
        {
            if (i == 0) {
                status = "w grze";
            }

            board->setNextSquareIt(squaresIt);

            if (i == score - 1) { // The player ends his round.
                change = (*squaresIt)->stayOn(money);

                // The player tries to start (for the next round - to check whether it will be possible).
                waitingRoundsLeft = (*squaresIt)->tryLeave(playerName);
                if (waitingRoundsLeft != 0) { // The player starts waiting.
                    status = "*** czekanie: " + std::to_string(waitingRoundsLeft) + " ***";
                    return;
                }

            }
            else { // The player continues playing.
                change = (*squaresIt)->goThrough(money);
            }

            if (change < 0 && money < (unsigned int)(-change)) { // The player went bankrupt :(
                while (i < score - 1) { // Set the sqaure to the player's destination.
                    i++;
                    board->setNextSquareIt(squaresIt);
                }
                setBankrupt();
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

    void setBankrupt() {
        isAlive = false;
        money = 0;
        status = "*** bankrut ***";
    }
};

#endif//WORLDCUP_PLAYER_H
