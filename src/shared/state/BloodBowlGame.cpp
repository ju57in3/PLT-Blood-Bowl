#include "BloodBowlGame.h"
#include "Setup.h"
#include "Kickoff.h"
#include "PlayerTurn.h"
#include "HalfTime.h"
#include "EndGame.h"

#include <iostream>
#include <iomanip>
#include <random>

namespace  state {

static void placeFromTeam(const Team& team, char mark, std::vector<std::string> &grid, int w, int h) {
    for (const Character &p : team.getCharacters()) {
        auto pos = p.getPosition(); // expected: pair<int,int>
        if (pos.first >= 0 && pos.first < w && pos.second >= 0 && pos.second < h) {
            char &cell = grid[pos.second][pos.first];
            if (cell == '.' || cell == 'O') cell = mark;
            else cell = '*'; // conflict marker
        }
    }
}

static void renderBoardAscii(std::ostream &os, const BloodBowlGame &game) {
    int w = game.getWidth();
    int h = game.getHeight();
    if (w <= 0 || h <= 0) {
        os << "(board not initialized)\n";
        return;
    }

    std::vector<std::string> grid(h, std::string(w, '.'));

    auto ball = game.getBallPosition();
    if (ball.first >= 0 && ball.first < w && ball.second >= 0 && ball.second < h) {
        grid[ball.second][ball.first] = 'O';
    }

    placeFromTeam(game.getTeamA(), 'A', grid, w, h);
    placeFromTeam(game.getTeamB(), 'B', grid, w, h);

    // print column indices
    os << "\nBoard (" << w << "x" << h << "):\n";
    os << "     ";
    for (int x = 0; x < w; ++x) os << (x % 10) << ' ';
    os << '\n';

    for (int y = h - 1; y >= 0; --y) {
        os << std::setw(2) << y << " | ";
        for (int x = 0; x < w; ++x) {
            os << grid[y][x];
            if (x < w - 1) os << ' ';
        }
        os << " |\n";
    }
}

    BloodBowlGame::BloodBowlGame(Team teamA, Team teamB)
        : teamA(std::move(teamA)),
          teamB(std::move(teamB)),
          currentTeam(this->coinToss()),
          turnCounter(0) {
        stateList.push_back(std::make_unique<Setup>(this));
        stateList.push_back(std::make_unique<Kickoff>(this));
        stateList.push_back(std::make_unique<PlayerTurn>(this));
        stateList.push_back(std::make_unique<HalfTime>(this));
        stateList.push_back(std::make_unique<EndGame>(this));
        currentState = stateList.at(SETUP).get(); // initial state
        width = 26;
        height = 15;
        ballIsHold = false;
    }

    void BloodBowlGame::setCurrentState(AbstractState* state) {
        currentState = state;
    }

    AbstractState* BloodBowlGame::getCurrentState() const {
        return currentState;
    }

    const std::vector<std::unique_ptr<AbstractState>>& BloodBowlGame::getStateList() const {
        return stateList;
    }

    Team BloodBowlGame::coinToss() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        int toss = dis(gen);
        if (toss == 0) {
            return this->teamA;
        } else {
            return this->teamB;
        }
    }

    Team BloodBowlGame::getTeamA() const{
        return teamA;
    }
    Team BloodBowlGame::getTeamB() const {
        return teamB;
    }

    Team BloodBowlGame::getCurrentTeam() const {
        return currentTeam;
    }

    void BloodBowlGame::setTurnCounter(int newTurnCount) {
        turnCounter = newTurnCount;
    }

    void BloodBowlGame::setCurrentTeam(Team team) {
        currentTeam = std::move(team);
    }

    std::pair<int,int> BloodBowlGame::getBallPosition() const {
        return ballPosition;
    }

    int BloodBowlGame::getWidth() const {
        return width;
    }

    int BloodBowlGame::getHeight() const{
        return height;
    }

    void BloodBowlGame::setBallPosition(std::pair<int,int> position) {
        ballPosition = position;
    }

    int BloodBowlGame::getTurnCounter() const {
        return turnCounter;
    }

    std::ostream& operator<<(std::ostream& os, const BloodBowlGame& game) {
        os << "\n=== GAME STATE ===\n";
        os << "Current State: " << (game.getCurrentState() ? typeid(*game.getCurrentState()).name() : "None") << "\n";
        os << "Turn Counter: " << game.getTurnCounter() << "\n";
        os << "Current Team: " << game.getCurrentTeam().getTeamId() << "\n";
        os << "Score - Team A: " << game.getTeamA().getScore() << " | Team B: " << game.getTeamB().getScore() << "\n";
        renderBoardAscii(os, game);
        os << "==================\n";
        return os;
    }
}
