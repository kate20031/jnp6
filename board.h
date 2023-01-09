#ifndef WORLDCUP_BOARD_H
#define WORLDCUP_BOARD_H

#include "die.h"
#include "square.h"
#include <list>
#include <memory>

class Board {
public:
    Board() : squares(), dies() {}

    explicit Board(std::list<std::shared_ptr<Square>> squares) : squares(std::move(squares)), dies() {}

    void addDie(const std::shared_ptr<Die>& die) {
        dies.push_back(die);
    }

    std::list<std::shared_ptr<Die>> getDies() {
        return dies;
    }

    std::list<std::shared_ptr<Square>> getSquares() {
        return squares;
    }

private:
    std::list<std::shared_ptr<Square>> squares;
    std::list<std::shared_ptr<Die>> dies;
};


#endif//WORLDCUP_BOARD_H
