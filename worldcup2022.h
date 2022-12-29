#ifndef WORLDCUP_H
#define WORLDCUP_H

#include <iostream>
#include <list>
#include <memory>
#include <map>
#include <string>
#include <set>

// TODO add pass by reference String
// TODO add exceptions!

using String = std::string;

class Die;

class ScoreBoard;

// TODO Add destructors for classes

std:: string getSquareName(int indexOfSquare) noexcept
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

// TODO Realize waiting when receive yellow card
// int stayOn(int indexOfSquare, int numberOfVisitors) noexcept
// {
//     switch (indexOfSquare)
//     {
//         case 1: return 50;
//         case 2: return (numberOfVisitors - 1) * 160;
//         case 3: return 0;
//         case 4: return (numberOfVisitors - 1) * 220;
//         case 5: return 0;
//         case 6: return (numberOfVisitors - 1) * 300 * 2.5;
//         case 7: return (numberOfVisitors - 1) * 280 * 2.5;
//         case 8: if (numberOfVisitors % 3 == 0) return 300;
//                 else return -300;
//         case 9: return (numberOfVisitors - 1) * 2.5 * 250;
//         case 10: return 120;
//         case 11: return (numberOfVisitors - 1) * 2.5 * 400;
//         case 12: return -180;
//         default: return 0;
//     }
// }

// int goThrough(int indexOfSquare) noexcept
// {
//     switch (indexOfSquare)
//     {
//         case 1: return 50;
//         case 2: return -160;
//         case 4: return -220;
//         case 6: return -300;
//         case 7: return -280;;
//         case 9: return -250;
//         case 11: return -400;
//         default: return 0;
//     }
// }

// Abstract class that represents a board field;
class Square {
    public:
        Square(String name) : 
            name(name) {}

        String getName() {
            return name;
        };

        virtual float stayOn() = 0;

        virtual float goThrough() = 0;

        // Returns true only if the square was left successfully.
        virtual bool tryLeave(Player * player) {
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

        virtual float stayOn() override {
            return stayOnValue;
        }

        virtual float goThrough() override {
            return goThroughValue;
        }

    private:
       int stayOnValue;
       int goThroughValue;
};

// TODO find better place:
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

        float stayOn() override {
            // Win.
            if (playersCounter == 0) {
                return winValue;
            }
            // Lose.
            else {
                return loseValue;
            }
            playersCounter = (playersCounter + 1) % 3;
        }

        // Neutral.
        float goThrough() override {
            return 0;
        }

    private:
        int winValue;
        int loseValue;
        int playersCounter;
};

// TODO: inherit from SimpleSquare?
class YellowCardSquare : public Square { // TODO: std::map<Player, int>()
    public:
        YellowCardSquare(String name, int waitingRounds) : 
            Square(name), waitingRounds(waitingRounds), waitingRoundsLeft() {}

        // TODO Is waiting only when stay?
        float stayOn() override {
            return 0;
        }

        // Neutral.
        float goThrough() override {
            return 0;
        }

        bool tryLeave(Player * player) override {
            auto it = waitingRoundsLeft.find(*player);
            if (it == waitingRoundsLeft.end()) { // Player starts waiting.
                waitingRoundsLeft[*player] = waitingRounds;
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
        std::map<Player, int> waitingRoundsLeft;
};

class GameSquare : public Square {
    public:
        GameSquare(String name, int fee, float importance) : 
            Square(name), fee(fee), importance(importance), feeCount(0) {}

        float stayOn() override {
            int tempFeeCount = feeCount;
            feeCount = 0;
            return feeCount * fee * importance;
        }
        
        // The player plays the match and pay the fee.
        float goThrough() override {
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

// END TODO;


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
    virtual void setScoreBoard(std::shared_ptr<ScoreBoard> scoreboard) {

    }

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

   Board(std::list<Square> squares) : squares(squares), dies() {
    //    this->squares = squares;
    //    this->dies = dies;
   }

   void addDie(std::shared_ptr<Die> die) {
        dies.push_back(die);
   }

   std::list<std::shared_ptr<Die>> getDies() {
       return dies;
   }
      
   void addSquare(Square &square) {
        squares.push_back(square);
   }

   std::list<Square> getSquares() {
       return squares;
   }

private:
   std::list<Square> squares;
   std::list<std::shared_ptr<Die>> dies;
};

// TODO create a class Board, add attributes dies and squares, hold Board as attribute
// TODO in Player instead of them;
// TODO Add 12 as constant attribute (sizeOfBoard) in class Board ???

class Player {
public:
    Player(String const &playerName, Board &board) : 
        playerName(playerName), money(1000), indexOfSquare(1), board(board), isAlive(true) {
        // this->playerName = playerName;
        // money = 1000;
        // indexOfSquare = 1;

    }

    void play() {
        unsigned short score;
        int change;

        std::list<std::shared_ptr<Die>> dies = board.getDies();
        std::list<Square> squares = board.getSquares();

        for (auto die : dies)
            score += die->roll();

        squaresIt = squares.begin();

        for (int i = 0; i < score && isAlive; i++) 
        {
            indexOfSquare = indexOfSquare == 12 ? 1 : indexOfSquare + 1;

            //Square square = *squaresIt;
            //square.addVisitor();
            //squares.erase(indexOfSquare);
            //squares.insert({indexOfSquare, square});

            if (i == score - 1) {
                //change = stayOn(indexOfSquare, square.getVisitors());
                change = squaresIt->stayOn();
                if (squaresIt->tryLeave(this)) { // Player can leave.
                    squaresIt++; // Player goes to next square in the board.
                    if (squaresIt == squares.end()) {
                        squaresIt = squares.begin(); // Board is cyclic.
                    }
                }
            } else {
                change = squaresIt->goThrough();
            }
            money += change;

            if (money < 0) { // Player went bankrupt :(
                 isAlive = false;
            }
        }    
    }

    std::string getName() {
        return playerName;
    }

    String getSquareName() {
        return squaresIt->getName();
    }

    float getMoney() {
        return money;
    }

    std::string getStatus() {
        return status;
    }

    bool getIsAlive() {
        return isAlive;
    }

    // static std::map<int, Square>& getSquares() {
    //     return squares;
    // }

private:
    std::string playerName;
    float money;
    std::string status;
    unsigned short indexOfSquare;
    Board board;
    bool isAlive;
    std::list<Square>::iterator squaresIt;
};


//std::map<int, Square> Player::squares;

class WorldCup2022 : public WorldCup {
public:
    WorldCup2022() : WorldCup() {
        std::list<Square> squares = std::list<Square>();

        squares.push_back(SeasonBeginningSquare(getSquareName(1)));
        squares.push_back(FriendlyGameSquare(getSquareName(2), 160));
        squares.push_back(DayOffSquare(getSquareName(3)));
        squares.push_back(FriendlyGameSquare(getSquareName(4), 220));
        squares.push_back(YellowCardSquare(getSquareName(5), 3));
        squares.push_back(PointsGameSquare(getSquareName(6), 300));
        squares.push_back(PointsGameSquare(getSquareName(7), 280));
        squares.push_back(BookmakerSquare(getSquareName(8), 300, 300));
        squares.push_back(PointsGameSquare(getSquareName(9), 250));
        squares.push_back(GoalSquare(getSquareName(10), 11));
        squares.push_back(FinalGameSquare(getSquareName(11), 400));
        squares.push_back(PenaltyKickSquare(getSquareName(12), 180));

        board = Board(squares);
    }

    ~WorldCup2022() = default;

    void addDie(std::shared_ptr<Die> die) override {
        //dies.push_back(die);
        board.addDie(die);
    }

    void addPlayer(std::string const &name) override {
        Player player = Player(name, board);
        players.push_back(player);
//        players.printAll();
    }

    void setScoreBoard(std::shared_ptr<ScoreBoard> scoreboard) override {
        this->scoreboard = scoreboard;
    }

    void play(unsigned int rounds) override {
        //this->rounds = rounds;

        for (int i = 1; i <= rounds; i++) {
            scoreboard->onRound(i - 1);

            for (auto player : players)
            {
                if(!player.getIsAlive()) {
                    deadPlayers.insert(player);
                    continue;
                }
                player.play();
                scoreboard->onTurn(player.getName(), player.getStatus(),
                                   player.getSquareName(), player.getMoney());
            }
            
        }
    }

    private:
        Board board;
        std::list<Player> players;
        // std::list<std::shared_ptr<Die>> dies;
        // std::map<int, Square> squares;
        int actual_round = 0;
        //unsigned int rounds;
        std::shared_ptr<ScoreBoard> scoreboard;
        std::set<Player> deadPlayers;
    };

//class Match {
//
//};

#endif