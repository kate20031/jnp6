#ifndef WORLDCUP_SQUARE_H
#define WORLDCUP_SQUARE_H

#include <iostream>
#include <map>
#include <string>

// Abstract class that represents the board field;
class Square {
public:
    explicit Square(std::string  name) : name(std::move(name)) {}

    virtual ~Square() = default;

    std::string getName() const {
        return name;
    }

    virtual double stayOn(int playersMoney) = 0;

    virtual double goThrough(int playersMoney) = 0;

    // Returns the number of rounds the player needs to wait.
    virtual int tryLeave([[maybe_unused]] const std::string& playerName) {
        return 0;
    }

private:
    const std::string name;
};

// Represents the board field with basic functionalities.
class SimpleSquare : public Square {
public:
    SimpleSquare(const std::string& name, int stayOnValue, int goThroughValue) :
        Square(name), stayOnValue(stayOnValue), goThroughValue(goThroughValue) {}

    double stayOn([[maybe_unused]] int playersMoney) override {
        return (double)stayOnValue;
    }

    double goThrough([[maybe_unused]] int playersMoney) override {
        return (double)goThroughValue;
    }

private:
    int stayOnValue;
    int goThroughValue;
};

class SeasonBeginningSquare : public SimpleSquare {
public:
    explicit SeasonBeginningSquare(const std::string& name) : SimpleSquare(name, 50, 50) {}
};

class DayOffSquare : public SimpleSquare {
public:
    explicit DayOffSquare(const std::string& name) : SimpleSquare(name, 0, 0) {}
};

class GoalSquare : public SimpleSquare {
public:
    GoalSquare(const std::string& name, int goalBonus) : SimpleSquare(name, goalBonus, 0) {}
};

class PenaltyKickSquare : public SimpleSquare {
public:
    PenaltyKickSquare(const std::string& name, int goalkeeperSalary) : SimpleSquare(name, -goalkeeperSalary, 0) {}
};

class BookmakerSquare : public Square {
public:
    BookmakerSquare(std::string name, int winValue, int loseValue) :
        Square(std::move(name)), winValue(winValue), loseValue(loseValue), playersCounter(0) {}

    double stayOn([[maybe_unused]] int playersMoney) override {
        playersCounter = (playersCounter + 1) % 3;
        // Win.
        if (playersCounter == 1)
            return (double)winValue;
        // Lose.
        else
            return (double)-loseValue;
    }

    // Neutral.
    double goThrough([[maybe_unused]] int playersMoney) override {
        return 0;
    }

private:
    int winValue;
    int loseValue;
    int playersCounter;
};

class YellowCardSquare : public SimpleSquare {
public:
    YellowCardSquare(const std::string& name, int waitingRounds) :
        SimpleSquare(name, 0, 0), waitingRounds(waitingRounds), waitingRoundsLeft() {}

    int tryLeave(const std::string& playerName) override {
        auto it = waitingRoundsLeft.find(playerName);
        if (it == waitingRoundsLeft.end()) { // The player starts waiting.
            waitingRoundsLeft[playerName] = waitingRounds;
            return 3; // Count of rounds the player needs to wait.
        }
        else { // The player has already been waiting.
            it->second--;
            if (it->second == 0) // The player stops waiting.
                waitingRoundsLeft.erase(it);
        }
        return it->second;
    }

private:
    int waitingRounds;
    std::map<std::string, int> waitingRoundsLeft;
};

class GameSquare : public Square {
public:
    GameSquare(std::string name, int fee, double importance) :
        Square(std::move(name)), fee(fee), importance(importance), feeSum(0) {}

    double stayOn([[maybe_unused]] int playersMoney) override {
        int tempFeeSum = feeSum;
        feeSum = 0;
        return (double)(tempFeeSum) * importance;
    }

    // The player plays the match and pay the fee.
    double goThrough(int playersMoney) override {
        if (playersMoney >= fee)
            feeSum += fee;
        else
            feeSum += playersMoney;
        return (double)-fee;
    }

private:
    int fee;
    double importance;
    int feeSum = 0;
};

class FriendlyGameSquare : public GameSquare {
public:
    FriendlyGameSquare(std::string name, int fee) : GameSquare(std::move(name), fee, 1) {}
};

class PointsGameSquare : public GameSquare {
public:
    PointsGameSquare(std::string name, int fee) : GameSquare(std::move(name), fee, 2.5) {}
};

class FinalGameSquare : public GameSquare {
public:
    FinalGameSquare(std::string name, int fee) : GameSquare(std::move(name), fee, 4) {}
};

#endif//WORLDCUP_SQUARE_H
