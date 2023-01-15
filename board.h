#ifndef WORLDCUP_BOARD_H
#define WORLDCUP_BOARD_H

#include "dice.h"
#include "die.h"
#include "square.h"
#include <list>
#include <memory>

class Board {
public:
    Board() : squares(), dice() {}

    explicit Board(std::list<std::shared_ptr<Square>> squares) : squares(std::move(squares)), dice() {}

    void addDie(const std::shared_ptr<Die>& die) {
        dice.addDie(die);
    }

    std::list<std::shared_ptr<Square>>::const_iterator getBeginSquares() const {
        return squares.begin();
    }

    unsigned short getRollScore() const {
        return dice.roll();
    }

    void setNextSquareIt(std::list<std::shared_ptr<Square>>::const_iterator& squareIt) const {
        squareIt++;
        if(squareIt == squares.end())
            squareIt = squares.begin(); // The board is cyclic.
    }



private:
    std::list<std::shared_ptr<Square>> squares;
    Dice dice;
};


#endif//WORLDCUP_BOARD_H
