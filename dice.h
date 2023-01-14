#ifndef WORLDCUP_DICE_H
#define WORLDCUP_DICE_H

#include "die.h"
#include "square.h"
#include <list>
#include <memory>

class Dice {
public:
    Dice() : dice() {}

    void addDie(const std::shared_ptr<Die>& die) {
        dice.push_back(die);
    }

    [[nodiscard]] unsigned short roll() const {
        unsigned short score = 0;

        if (dice.size() > diceNumber) {
            throw TooManyDiceException();
        }
        if (dice.size() < diceNumber) {
            throw TooFewDiceException();
        }

        for (const auto& die : dice) {
            score += die->roll();
        }

        return score;
    }

private:
    std::list<std::shared_ptr<Die>> dice;
    static const int diceNumber = 2; 
};


#endif//WORLDCUP_DICE_H