#ifndef WORLDCUP_H
#define WORLDCUP_H

#include <iostream>
#include <list>
#include <memory>
#include <map>
#include <string>


class Die;

class ScoreBoard;

// TODO Add destructors for classes

std:: string getSquareName(int indexOfSquare) noexcept
{
    switch (indexOfSquare)
    {
        case 1: return "Początek sezonu";
        case 2 : return "Mecz z San Marino";
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
    }
}

// TODO Realize waiting when receive yellow card
int stayOn(int indexOfSquare, int numberOfVisitors) noexcept
{
    switch (indexOfSquare)
    {
        case 1: return 50;
        case 2 : return (numberOfVisitors - 1) * 160;
        case 3: return 0;
        case 4: return (numberOfVisitors - 1) * 220;
        case 5: return 0;
        case 6: return (numberOfVisitors - 1) * 300 * 2.5;
        case 7: return (numberOfVisitors - 1) * 280 * 2.5;
        case 8: if (numberOfVisitors % 3 == 0) return 300;
            else return -300;
        case 9: return (numberOfVisitors - 1) * 2.5 * 250;
        case 10: return 120;
        case 11: return (numberOfVisitors - 1) * 2.5 * 400;
        case 12: return -180;
    }
}

int goThrough(int indexOfSquare) noexcept
{
    switch (indexOfSquare)
    {
        case 1: return 50;
        case 2 : return -160;
        case 4: return -220;
        case 6: return -300;
        case 7: return -280;;
        case 9: return -250;
        case 11: return -400;
        default: return 0;
    }
}

class Square {
    public:
        Square(int index) {
//            this->index = index;
            name = getSquareName(index);
        }

        std:: string getName() {
                return name;
        };

        void addVisitor() {
            numberOfVisitors++;
        }

        int getVisitors() {
            return numberOfVisitors;
        }

    private:
//        int index;
        std:: string name;
        int numberOfVisitors = 0;
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

//class Board {
//public:
//    Board(int numberOfSquares, std::list<std::shared_ptr<Die>> dies) {
//        this->numberOfSquares = numberOfSquares;
//        this->dies = dies;
//    }
//
//    int getNumberOfSquares() {
//        return numberOfSquares;
//    }
//
//    std::list<std::shared_ptr<Die>> getDies() {
//        return dies;
//    }
//private:
//    int numberOfSquares;
//    std::list<std::shared_ptr<Die>> dies;
//};

// TODO create a class Board, add attributes dies and squares, hold Board as attribute
// TODO in Player instead of them;
// TODO Add 12 as constant attribute (sizeOfBoard) in class Board ???

class Player {
public:
    Player(std::string const &playerName, std::list<std::shared_ptr<Die>> dies, std::map<int, Square> squares) {
        this->playerName = playerName;
        this->dies = dies;
        money = 1000;
        indexOfSquare = 1;
        this->squares = squares;
    }

    void play() {
        unsigned short score;
        int change;
        int rolls = 0;

        for (auto it = dies.begin(); it != dies.end(); it ++) {
            score = (*it)->roll();;
            rolls++;

            for (int i = 0; i < score; i++) {

                indexOfSquare++;
                if (indexOfSquare > 12) {
                    indexOfSquare = (indexOfSquare % 12) + 1;
                }

                Square square = squares.at(indexOfSquare);;
                square.addVisitor();
                squares.erase(indexOfSquare);
                squares.insert({indexOfSquare, square});

                if (rolls == 2 && i == score - 1) {
                    change = stayOn(indexOfSquare, square.getVisitors());
                } else {
                    change = goThrough(indexOfSquare);
                    //            if (money + chnge < 0) // TODO bankrutuje
                }
                money += change;
            }
        }
    }

    std::string getName() {
        return playerName;
    }

    Square getSquare() {
        return squares.at(indexOfSquare);
    }

    int getMoney() {
        return money;
    }

    std::string getStatus() {
        return status;
    }

    static std::map<int, Square>& getSquares() {
        return squares;
    }

private:
    std::string playerName;
    unsigned int money;
    std::string status;
    unsigned short indexOfSquare;
//    Board board;
    std::list<std::shared_ptr<Die>> dies;
    static std::map<int, Square> squares;
};


std::map<int, Square> Player::squares;

class WorldCup2022 : public WorldCup {
public:
    WorldCup2022() {
        for (int i = 1; i <= 12; i ++) {
            squares.insert({i, Square(i)});
        }
    }

    virtual ~WorldCup2022() = default;

    virtual void addDie(std::shared_ptr<Die> die) override {
        dies.push_back(die);
    }

    virtual void addPlayer(std::string const &name) override {
        Player player = Player(name, dies, squares);
        players.push_back(player);
//        players.printAll();
    }

    virtual void setScoreBoard(std::shared_ptr<ScoreBoard> scoreboard) override {
        this->scoreboard = scoreboard;
    }

    virtual void play(unsigned int rounds) override {
        this->rounds = rounds;

        for (int i = 1; i <= rounds; i++) {
            scoreboard->onRound(i - 1);

            for (int i = 0; i < players.size(); i++) {
                Player player = players.front();
                player.play();
                scoreboard->onTurn(player.getName(), player.getStatus(),
                                   player.getSquare().getName(), player.getMoney());
                players.pop_front();
                players.push_back(player);
            }
        }
    }

    private:
//        Board board;
        std::list<Player> players;
        std::list<std::shared_ptr<Die>> dies;
        std::map<int, Square> squares;
        int actual_round = 0;
        unsigned int rounds;
        std::shared_ptr<ScoreBoard> scoreboard;
    };

//class Match {
//
//};

#endif