#include "BloodBowlGame.h"
#include "Setup.h"
#include "Kickoff.h"
#include "PlayerTurn.h"
#include "HalfTime.h"
#include "EndGame.h"

#include <iostream>
#include <iomanip>

namespace  state {

static void placeFromTeam(Team team, char mark, std::vector<std::string> &grid, int w, int h) {
    for (Character &p : team.getCharacters()) {
        auto pos = p.getPosition(); // expected: pair<int,int>
        if (pos.first >= 0 && pos.first < w && pos.second >= 0 && pos.second < h) {
            char &cell = grid[pos.second][pos.first];
            if (cell == '.' || cell == 'O') cell = mark;
            else cell = '*'; // conflict marker
        }
    }
}

static void renderBoardAscii(std::ostream &os, BloodBowlGame &game) {
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
    os << "   ";
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
        :
    teamA(teamA),
    teamB(teamB),
    currentTeam(coinToss()),
    turnCounter(0){
        statelist.push_back(new Setup(this));
        statelist.push_back(new Kickoff(this));
        statelist.push_back(new PlayerTurn(this));
        statelist.push_back(new HalfTime(this));
        statelist.push_back(new EndGame(this));
        currentState = statelist.at(SETUP);

        width = 26;
        height = 15;
        ballIsHold = false;

    }

    void BloodBowlGame::setCurrentState(AbstractState *state) {
        currentState = state;
    }

    AbstractState *BloodBowlGame::getCurrentState() {
        return currentState;
    }

    Team BloodBowlGame::coinToss() {
        int toss = rand() % 2;
        if (toss == 0) {
            return this->teamA;
        } else {
            return this->teamB;
        }
    }

    Team BloodBowlGame::getTeamA() {
        return teamA;
    }
    Team BloodBowlGame::getTeamB() {
        return teamB;
    }

    Team BloodBowlGame::getCurrentTeam() {
        return currentTeam;
    }

    void BloodBowlGame::setTurnCounter(int newTurnCount) {
        turnCounter = newTurnCount;
    }

    void BloodBowlGame::setCurrentTeam(Team team) {
        currentTeam = team;
    }

    std::pair<int,int> BloodBowlGame::getBallPosition() {
        return ballPosition;
    }

    int BloodBowlGame::getWidth() {
        return width;
    }

    int BloodBowlGame::getHeight() {
        return height;
    }

    void BloodBowlGame::setBallPosition(std::pair<int,int> position) {
        ballPosition = position;
    }

    std::vector<AbstractState*> BloodBowlGame::getStateList() {
        return statelist;
    }

    int BloodBowlGame::getTurnCounter() {
        return turnCounter;
    }

    std::ostream& operator<<(std::ostream& os, BloodBowlGame& game) {
        os << "\n=== GAME STATE ===\n";
        os << "Current State: " << game.getCurrentState() << "\n";
        os << "Turn Counter: " << game.getTurnCounter() << "\n";
        os << "Current Team: " << game.getCurrentTeam().getTeamId() << "\n";
        os << "Score - Team A: " << game.getTeamA().getScore() << " | Team B: " << game.getTeamB().getScore() << "\n";
        
        renderBoardAscii(os, game);

        os << "==================\n";

        return os;
    }
}
