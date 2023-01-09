#ifndef WORLDCUP_EXCEPTIONS_H
#define WORLDCUP_EXCEPTIONS_H

#include <exception>

class DiceException : public std::exception {
public:
    virtual const char* what() const noexcept = 0;
};

class PlayerException : public std::exception {
public:
    virtual const char* what() const noexcept = 0;
};

class TooManyDiceException : public DiceException {
public:
    const char* what() const noexcept override {
        return "Too many dice";
    }
};

class TooFewDiceException : public DiceException {
public:
    const char* what() const noexcept override {
        return "Too few dice";
    }
};

class TooManyPlayersException : public PlayerException {
public:
    const char* what() const noexcept override {
        return "Too many player";
    }
};

class TooFewPlayersException : public PlayerException {
public:
    const char* what() const noexcept override {
        return "Too few player";
    }
};

#endif//WORLDCUP_EXCEPTIONS_H
