#ifndef WORLDCUP_H
#define WORLDCUP_H

#include "exceptions.h"
#include "player.h"
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>

class ScoreBoard;

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

class WorldCup2022 : public WorldCup {
public:
    WorldCup2022() : WorldCup() {
        std::list<std::shared_ptr<Square>> squares = std::list<std::shared_ptr<Square>>();

        squares.push_back(std::make_shared<SeasonBeginningSquare>("Początek sezonu"));
        squares.push_back(std::make_shared<FriendlyGameSquare>("Mecz z San Marino", 160));
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

        board = std::make_shared<Board>(squares);

        deadPlayersCount = 0;
    }

    ~WorldCup2022() override = default;

    void addDie(std::shared_ptr<Die> die) override {
        board->addDie(die);
    }

    void addPlayer(std::string const &name) override {
        players.push_back(std::make_shared<Player>(name, board));
    }

    void setScoreBoard(std::shared_ptr<ScoreBoard> scoreboard) override {
        this->scoreboard = scoreboard;
    }

    std::shared_ptr<Player> findWinner(std::list<std::shared_ptr<Player>> players) {
        std::shared_ptr<Player> winner = nullptr;

        for (const auto& player : players) {
          if (winner == nullptr || player->getMoney() > winner -> getMoney()) {
            winner = player;// New potential winner.
          }
        }

        return winner;
    }

    void play(unsigned int rounds) override {
        std::shared_ptr<Player> winner;

        // Count of players is out of range
        if (players.size() < 2) {
            throw TooFewPlayersException();
        } else if (players.size() > 11) {
            throw TooManyPlayersException();
        }

        for (unsigned int i = 1; i <= rounds && deadPlayersCount < players.size() - 1; i++) {
            scoreboard->onRound(i - 1);

            for (const auto& player : players) {
                if(!player->getIsAlive())
                    continue;

                player->play();
                scoreboard->onTurn(player->getName(), player->getStatus(),
                                   player->getSquareName(),player->getMoney());

                if(!player->getIsAlive())
                    deadPlayersCount++;

                if (deadPlayersCount >= players.size() - 1) {
                    winner = findWinner(players);
                    scoreboard->onWin(winner->getName());
                    return;
                }
            }

        }

        winner = findWinner(players);
        scoreboard->onWin(winner->getName());
    }

private:
    std::shared_ptr<Board> board;
    std::list<std::shared_ptr<Player>> players;
    std::shared_ptr<ScoreBoard> scoreboard;
    long unsigned int deadPlayersCount;
};

#endif
