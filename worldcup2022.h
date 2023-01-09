#ifndef WORLDCUP_H
#define WORLDCUP_H

#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

// TODO Add more custom types (e.g. for lists of shared pointers)
// TODO Add const where possible.
// TODO (later) Think about better managment of Board attributes.

// TODO In separate file.
using String = std::string;

const unsigned int INITIAL_MONEY = 1000;

class Die;

class ScoreBoard;

class Player;

// TODO Add destructors for classes

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

// Abstract class that represents a board field;
class Square {
public:
    explicit Square(String  name) : name(std::move(name)) {}

    String getName() {
        return name;
    };

    virtual double stayOn() = 0;

    virtual double goThrough() = 0;

    // Returns true only if the square was left successfully.
    virtual int tryLeave(const String& playerName) {
        // TODO Fix unused parameter warning.
        return 0;
    }

private:
    String name;
};

// Represents a board field;
class SimpleSquare : public Square {
public:
    SimpleSquare(const String& name, int stayOnValue, int goThroughValue) : 
        Square(name), stayOnValue(stayOnValue), goThroughValue(goThroughValue) {}

    double stayOn() override {
        return (double)stayOnValue;
    }

    double goThrough() override {
        return (double)goThroughValue;
    }

private:
    int stayOnValue;
    int goThroughValue;
};

class SeasonBeginningSquare : public SimpleSquare {
public:
    explicit SeasonBeginningSquare(const String& name) : SimpleSquare(name, 50, 50) {}
};

class DayOffSquare : public SimpleSquare {
public:
    explicit DayOffSquare(const String& name) : SimpleSquare(name, 0, 0) {}
};

class GoalSquare : public SimpleSquare {
public:
    GoalSquare(const String& name, int goalBonus) : SimpleSquare(name, goalBonus, 0) {}
};

class PenaltyKickSquare : public SimpleSquare {
public:
    PenaltyKickSquare(const String& name, int goalkeeperSalary) : SimpleSquare(name, -goalkeeperSalary, 0) {}
};

class BookmakerSquare : public Square {
public:
    BookmakerSquare(String name, int winValue, int loseValue) :
        Square(std::move(name)), winValue(winValue), loseValue(loseValue), playersCounter(0) {}

    double stayOn() override {
        // Win.
        if (playersCounter == 0) {
            playersCounter = (playersCounter + 1) % 3;
            return (double)winValue;
        }
        // Lose.
        else {
            playersCounter = (playersCounter + 1) % 3;
            return (double)-loseValue;
        }
    }

    // Neutral.
    double goThrough() override {
        return 0;
    }

private:
    int winValue;
    int loseValue;
    int playersCounter;
};

class YellowCardSquare : public SimpleSquare {
public:
    YellowCardSquare(const String& name, int waitingRounds) :
        SimpleSquare(name, 0, 0), waitingRounds(waitingRounds), waitingRoundsLeft() {}

    int tryLeave(const String& playerName) override {
        auto it = waitingRoundsLeft.find(playerName);
        if (it == waitingRoundsLeft.end()) { // Player starts waiting.
            waitingRoundsLeft[playerName] = waitingRounds;
            return 3; // Count of rounds the player needs to wait.
        }
        else {
            it->second--;
            if (it->second == 0) { // Time to leave.
                waitingRoundsLeft.erase(it);
            }
        }
        return it->second;
    }

private:
    int waitingRounds;
    std::map<String, int> waitingRoundsLeft;
};

class GameSquare : public Square {
public:
    GameSquare(String name, int fee, double importance) :
        Square(std::move(name)), fee(fee), importance(importance), feeCount(0) {}

    double stayOn() override {
        int tempFeeCount = feeCount;
        feeCount = 0;
        return (double)(tempFeeCount * fee) * importance;
    }

    // The player plays the match and pay the fee.
    double goThrough() override {
        feeCount++;
        return (double)-fee;
    }

private:
    int fee;
    double importance;
    int feeCount;
};

class FriendlyGameSquare : public GameSquare {
public:
    FriendlyGameSquare(String name, int fee) : GameSquare(std::move(name), fee, 1) {}
};

class PointsGameSquare : public GameSquare {
public:
    PointsGameSquare(String name, int fee) : GameSquare(std::move(name), fee, 2.5) {}
};

class FinalGameSquare : public GameSquare {
public:
    FinalGameSquare(String name, int fee) : GameSquare(std::move(name), fee, 4) {}
};


// Reprezentuje interfejs gry WorldCup.
// Konkretną implementacją jest WorldCup2022.
// WorldCup2022 ma mieć konstruktor bezparametrowy.
class WorldCup {
public:
    virtual ~WorldCup() = default;

    // Jeżeli argumentem jest pusty wskaźnik, to nie wykonuje żadnej operacji
    // (ale nie ma błędu).
    virtual void addDie(std::shared_ptr<Die> die) = 0;

    // Dodaje nowego gracza o podanej nazwie.
    virtual void addPlayer(std::string const &name) = 0;

    // Konfiguruje tablicę wyników. Domyślnie jest skonfigurowana tablica
    // wyników, która nic nie robi.
    virtual void setScoreBoard(std::shared_ptr<ScoreBoard> scoreboard) = 0;

    // Przeprowadza rozgrywkę co najwyżej podanej liczby rund (rozgrywka może
    // skończyć się wcześniej).
    // Jedna runda obejmuje po jednym ruchu każdego gracza.
    // Gracze ruszają się w kolejności, w której zostali dodani.
    // Na początku każdej rundy przekazywana jest informacja do tablicy wyników
    // o początku rundy (onRound), a na zakończenie tury gracza informacje
    // podsumowujące dla każdego gracza (onTurn).
    // Rzuca TooManyDiceException, jeśli jest zbyt dużo kostek.
    // Rzuca TooFewDiceException, jeśli nie ma wystarczającej liczby kostek.
    // Rzuca TooManyPlayersException, jeśli jest zbyt dużo graczy.
    // Rzuca TooFewPlayersException, jeśli liczba graczy nie pozwala na
    // rozpoczęcie gry.
    // Wyjątki powinny dziedziczyć po std::exception.
    virtual void play(unsigned int rounds) = 0;
};

// Reprezentuje interfejs kostki do gry.
// Konkretna implementacja zostanie dostarczona przy testach.
class Die {
public:
    virtual ~Die() = default;

    // Zwraca wynik rzutu kostką.
    [[nodiscard]] virtual unsigned short roll() const = 0;
};

// Reprezentuje tablicę wyników.
class ScoreBoard {
public:
    virtual ~ScoreBoard() = default;

    // Wywoływane, aby zasygnalizować początek rundy.
    virtual void onRound(unsigned int roundNo) = 0;

    // Wywoływane po skończeniu kolejki danego gracza, aby przekazać informacje o aktualnym stanie gracza.
    virtual void onTurn(std::string const &playerName, std::string const &playerStatus,
                        std::string const &squareName, unsigned int money) = 0;

    // Wywoływane, aby zasygnalizować zwycięstwo danego gracza.
    virtual void onWin(std::string const &playerName) = 0;
};

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

class Player {
public:
    Player(String playerName, Board &board) :
        playerName(std::move(playerName)), money(INITIAL_MONEY), status("w grze"), board(board), isAlive(true) {

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

            printf("%s   %s\n", playerName.c_str(), (*squaresIt)->getName().c_str());

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

    String getName() {
        return playerName;
    }

    String getSquareName() {
        return (*squaresIt)->getName();
    }

    unsigned int getMoney() const {
        return money;
    }

    String getStatus() {
        return status;
    }

    bool getIsAlive() const {
        return isAlive;
    }

private:
    String playerName;
    unsigned int money;
    String status;
    Board board;
    bool isAlive;
    std::list<std::shared_ptr<Square>>::iterator squaresIt;
    std::list<std::shared_ptr<Die>> dies;
    std::list<std::shared_ptr<Square>> squares;
};


class WorldCup2022 : public WorldCup {
public:
    WorldCup2022() : WorldCup() {
        std::list<std::shared_ptr<Square>> squares = std::list<std::shared_ptr<Square>>();

        squares.push_back(std::make_shared<SeasonBeginningSquare>("Początek sezonu"));
        squares.push_back(std::make_shared<FriendlyGameSquare("Mecz z San Marino", 160));
        squares.push_back(std::make_shared<DayOffSquare>("Dzień wolny od treningu"));
        squares.push_back(std::make_shared<FriendlyGameSquare>("Mecz z Liechtensteinem", 220));
        squares.push_back(std::make_shared<YellowCardSquare>("Żółta kartka", 3));
        squares.push_back(std::make_shared<PointsGameSquare>("Mecz z Meksykiem", 300));
        squares.push_back(std::make_shared<PointsGameSquare>("Mecz z Arabią Saudyjską", 280));
        squares.push_back(std::make_shared<BookmakerSquare>("Bukmacher", 100, 100));
        squares.push_back(std::make_shared<PointsGameSquare>("Mecz z Argentyną", 250));
        squares.push_back(std::make_shared<GoalSquare>("Gol", 120));
        squares.push_back(std::make_shared<FinalGameSquare>("Mecz z Francją", 400));
        squares.push_back(std::make_shared<PenaltyKickSquare>("Rzut karny", 180));

        board = Board(squares);
    }

    ~WorldCup2022() override = default;

    void addDie(std::shared_ptr<Die> die) override {
        board.addDie(die);
    }

    void addPlayer(std::string const &name) override {
        players.push_back(std::make_shared<Player>(name, board));
    }

    void setScoreBoard(std::shared_ptr<ScoreBoard> scoreboard) override {
        this->scoreboard = scoreboard;
    }

    void play(unsigned int rounds) override {

        // Count of players is out of range
        if (players.size() < 2) {
            throw TooFewPlayersException();
        } else if (players.size() > 11) {
            throw TooManyPlayersException();
        }

        for (unsigned int i = 1; i <= rounds && deadPlayers.size() < players.size() - 1; i++) {
            scoreboard->onRound(i - 1);

            for (const auto& player : players)
            {
                if(!player->getIsAlive())
                    continue;

                player->play();
                scoreboard->onTurn(player->getName(), player->getStatus(),
                                   player->getSquareName(),player->getMoney());

                if(!player->getIsAlive())
                    deadPlayers.insert(player->getName());
            }

        }

        std::shared_ptr<Player> winner = nullptr;
        for (const auto& player : players) {
            if (winner == nullptr || player->getMoney() > winner -> getMoney()) {
                winner = player;// New potentian winner.
            }
        }

        scoreboard->onWin(winner->getName());
    }

private:
    Board board;
    std::list<std::shared_ptr<Player>> players;
    std::shared_ptr<ScoreBoard> scoreboard;
    std::unordered_set<String> deadPlayers;
};

#endif
