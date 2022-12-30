#ifndef WORLDCUP_H
#define WORLDCUP_H

#include <iostream>
#include <list>
#include <memory>
#include <map>
#include <string>
#include <unordered_set>

// TODO add exceptions!
// TODO Finish player status (mainly waiting count).
// TODO Add more custom types (e.g. for lists of shared pointers)
// TODO Add const where possible.
// TODO (later) Think about better managment of Board attributes.

using String = std::string;

const unsigned int INITIAL_MONEY = 1000;

class Die;

class ScoreBoard;

class Player;

// TODO Add destructors for classes


// Abstract class that represents a board field;
class Square {
    public:
        Square(const String& name) : 
            name(name) {}

        String getName() {
            return name;
        };

        virtual int stayOn() = 0;

        virtual int goThrough() = 0;

        // Returns true only if the square was left successfully.
        virtual bool tryLeave(const String& playerName) {
            // TODO Fix unused parameter warning.
            return true;
        }

    private:
       String name;
};

// Represents a board field;
class SimpleSquare : public Square {
    public:
        SimpleSquare(String name, int stayOnValue, int goThroughValue) : Square(name), 
            stayOnValue(stayOnValue), goThroughValue(goThroughValue) {}

        virtual int stayOn() override {
            return stayOnValue;
        }

        virtual int goThrough() override {
            return goThroughValue;
        }

    private:
       int stayOnValue;
       int goThroughValue;
};

class SeasonBeginningSquare : public SimpleSquare {
    public:
        SeasonBeginningSquare(String name) : 
            SimpleSquare(name, 50, 50) {}
};

class DayOffSquare : public SimpleSquare {
    public:
        DayOffSquare(String name) : 
            SimpleSquare(name, 0, 0) {}
};

class GoalSquare : public SimpleSquare {
    public:
        GoalSquare(String name, int goalBonus) : 
            SimpleSquare(name, goalBonus, 0) {}
};

class PenaltyKickSquare : public SimpleSquare {
    public:
        PenaltyKickSquare(String name, int goalkeeperSalary) : 
            SimpleSquare(name, -goalkeeperSalary, 0) {}
};

class BookmakerSquare : public Square {
    public:
        BookmakerSquare(String name, int winValue, int loseValue) : 
            Square(name), winValue(winValue), loseValue(loseValue), playersCounter(0) {}

        int stayOn() override {
            // Win.
            if (playersCounter == 0) {
                return winValue;
            }
            // Lose.
            else {
                return -loseValue;
            }
            playersCounter = (playersCounter + 1) % 3;
        }

        // Neutral.
        int goThrough() override {
            return 0;
        }

    private:
        int winValue;
        int loseValue;
        int playersCounter;
};

class YellowCardSquare : public SimpleSquare { // TODO: std::map<Player, int>()
    public:
        YellowCardSquare(String name, int waitingRounds) : 
            SimpleSquare(name, 0, 0), waitingRounds(waitingRounds), waitingRoundsLeft() {}

        bool tryLeave(const String& playerName) override {
            auto it = waitingRoundsLeft.find(playerName);
            if (it == waitingRoundsLeft.end()) { // Player starts waiting.
                waitingRoundsLeft[playerName] = waitingRounds;
            }
            else {
                it -> second--;
                if (it -> second == 0) { // Time to leave.
                    waitingRoundsLeft.erase(it);
                    return true;
                }
            }
            return false;
        }

    private:
        int waitingRounds;
        std::map<String, int> waitingRoundsLeft;
};

class GameSquare : public Square {
    public:
        GameSquare(String name, int fee, float importance) : 
            Square(name), fee(fee), importance(importance), feeCount(0) {}

        int stayOn() override {
            int tempFeeCount = feeCount;
            feeCount = 0;
            return tempFeeCount * fee * importance;
        }
        
        // The player plays the match and pay the fee.
        int goThrough() override {
            feeCount++;
            return -fee;
        }

    private:
        int fee;
        float importance;
        int feeCount;
};

class FriendlyGameSquare : public GameSquare {
    public:
        FriendlyGameSquare(String name, int fee) :
            GameSquare(name, fee, 1) {}
};

class PointsGameSquare : public GameSquare {
    public:
        PointsGameSquare(String name, int fee) :
            GameSquare(name, fee, 2.5) {}
};

class FinalGameSquare : public GameSquare {
    public:
        FinalGameSquare(String name, int fee) :
            GameSquare(name, fee, 4) {}
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

   Board(std::list<std::shared_ptr<Square>> squares) : squares(squares), dies() {}

   void addDie(std::shared_ptr<Die> die) {
        dies.push_back(die);
   }

   std::list<std::shared_ptr<Die>> getDies() {
       return dies;
   }
      
   void addSquare(std::shared_ptr<Square> square) {
        squares.push_back(square);
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
    Player(String const &playerName, Board &board) : 
        playerName(playerName), money(INITIAL_MONEY), status("w grze"), indexOfSquare(1), board(board), isAlive(true) {
            dies = board.getDies();
            squares = board.getSquares();
            squaresIt = squares.begin();
        }

    void play() {
        unsigned short score = 0;
        int change;

        for (auto die : dies)
            score += die->roll();

        // if (squaresIt)
        //     std::cerr << "SCORE: " << score << std::endl;

        for (int i = 0; i < score && isAlive; i++) 
        {
            if (i == 0) {
                bool canLeave = (*squaresIt)->tryLeave(playerName); // Player tries to start. 
                if (!canLeave) { // Player have to wait.
                    status = "*** czekanie ***";
                    return;
                }
                else {
                    status = "w grze";
                }
            }

            squaresIt++; // Player goes to the next square in the board.
            if (squaresIt == squares.end()) {
                squaresIt = squares.begin(); // Board is cyclic.
            }

            if (i == score - 1) { // Player ends his round.
                change = (*squaresIt)->stayOn();
            } 
            else {
                change = (*squaresIt)->goThrough();
            }
            
            if (change < 0 && money < (unsigned int)(-change)) { // Player went bankrupt :(
                 isAlive = false;
                 status = "*** bankrut ***";
            }
            else
                money += change;
        }    
    }

    String getName() {
        return playerName;
    }

    String getSquareName() {
        return (*squaresIt)->getName();
    }

    float getMoney() {
        return money;
    }

    String getStatus() {
        return status;
    }

    bool getIsAlive() {
        return isAlive;
    }

private:
    String playerName;
    unsigned int money;
    String status;
    unsigned short indexOfSquare;
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

        squares.push_back(std::make_shared<SeasonBeginningSquare>(getSquareName(1)));
        squares.push_back(std::make_shared<FriendlyGameSquare>(getSquareName(2), 160));
        squares.push_back(std::make_shared<DayOffSquare>(getSquareName(3)));
        squares.push_back(std::make_shared<FriendlyGameSquare>(getSquareName(4), 220));
        squares.push_back(std::make_shared<YellowCardSquare>(getSquareName(5), 3));
        squares.push_back(std::make_shared<PointsGameSquare>(getSquareName(6), 300));
        squares.push_back(std::make_shared<PointsGameSquare>(getSquareName(7), 280));
        squares.push_back(std::make_shared<BookmakerSquare>(getSquareName(8), 300, 300));
        squares.push_back(std::make_shared<PointsGameSquare>(getSquareName(9), 250));
        squares.push_back(std::make_shared<GoalSquare>(getSquareName(10), 120));
        squares.push_back(std::make_shared<FinalGameSquare>(getSquareName(11), 400));
        squares.push_back(std::make_shared<PenaltyKickSquare>(getSquareName(12), 180));

        board = Board(squares);
    }

    ~WorldCup2022() = default;

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

        for (unsigned int i = 1; i <= rounds && deadPlayers.size() < players.size() - 1; i++) {
            scoreboard->onRound(i - 1);

            for (auto player : players)
            {
                if(!player->getIsAlive()) {
                    deadPlayers.insert(player->getName());
                    continue;
                }
                player->play();
                scoreboard->onTurn(player->getName(), player->getStatus(),
                                   player->getSquareName(),player->getMoney());
            }
            
        }

        std::shared_ptr<Player> winner = NULL;
        for (auto player : players) {
            if (winner == NULL || player->getMoney() > winner -> getMoney()) {
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

    String getSquareName(int indexOfSquare) noexcept
    {
        switch (indexOfSquare)
        {
            case 1: return "Początek sezonu";
            case 2: return "Mecz z San Marino";
            case 3: return "Dzień wolny od treningu";
            case 4: return "Mecz z Lichtensteinem";
            case 5: return "Żółta kartka";
            case 6: return "Mecz z Meksykiem";
            case 7: return "Mecz z Arabią Saudyjską";
            case 8: return "Bukmacher";
            case 9: return "Mecz z Argentyną";
            case 10: return "Gol";
            case 11: return "Mecz z Francją";
            case 12: return "Rzut karny";
            default: return "ERROR";
        }
    }
};

#endif