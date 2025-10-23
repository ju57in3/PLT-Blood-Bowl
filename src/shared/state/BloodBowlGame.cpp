#include "BloodBowlGame.h"

#include <filesystem>

#include "Setup.h"
#include "Kickoff.h"
#include "PlayerTurn.h"
#include "HalfTime.h"
#include "EndGame.h"

#include <iostream>
#include <iomanip>
#include <random>

namespace  state {

    const char* stateToString(const BloodBowlGame& game)  {
        if (game.getCurrentState() == game.getStateList().at(SETUP).get()) {
            return "Setup";
        } else if (game.getCurrentState() == game.getStateList().at(KICKOFF).get()) {
            return "Kickoff";
        } else if (game.getCurrentState() == game.getStateList().at(PLAYERTURN).get()) {
            return "PlayerTurn";
        } else if (game.getCurrentState() == game.getStateList().at(HALFTIME).get()) {
            return "HalfTime";
        } else if (game.getCurrentState() == game.getStateList().at(ENDGAME).get()) {
            return "EndGame";
        } else {
            return "Unknown State";
        }
    }

    static void placeFromTeam(const Team& team, char mark, std::vector<std::string> &grid, int w, int h) {
        for (const auto& pptr : team.getCharacters()) {
            if (pptr != nullptr) {
                const Character& p = *pptr;
                auto pos = p.getPosition(); // expected: pair<int,int>
                if (pos.first >= 0 && pos.first < w && pos.second >= 0 && pos.second < h) {
                    char &cell = grid[pos.second][pos.first];
                    if (cell == '.' || cell == 'O') cell = mark;
                    else cell = '*'; // conflict marker
                }
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

    BloodBowlGame::BloodBowlGame(Team& teamA, Team& teamB)
        : teamA(teamA),
          teamB(teamB),
    currentTeam(&teamA),
    turnCounter(0) { // Initialize currentTeam with a const reference
        stateList.push_back(std::make_unique<Setup>(this));
        stateList.push_back(std::make_unique<Kickoff>(this));
        stateList.push_back(std::make_unique<PlayerTurn>(this));
        stateList.push_back(std::make_unique<HalfTime>(this));
        stateList.push_back(std::make_unique<EndGame>(this));
        currentState = stateList.at(SETUP).get(); // initial state
        width = 26;
        height = 15;
        ballIsHold = false;
        nb_repetition = 0;
        nb_repetition_max = 3;
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

    Team* BloodBowlGame::coinToss() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        int toss = dis(gen);
        return (toss == 0) ? &teamA : &teamB;
    }

    Team& BloodBowlGame::getTeamA() const {
        return teamA;
    }

    Team& BloodBowlGame::getTeamB() const {
        return teamB;
    }

    Team* BloodBowlGame::getCurrentTeam() const {
        return currentTeam;
    }

    void BloodBowlGame::setCurrentTeam(Team* team) {
        currentTeam = team;
    }

    int BloodBowlGame::getWidth() const {
        return width;
    }

    int BloodBowlGame::getHeight() const{
        return height;
    }

    void BloodBowlGame::setBallPosition(std::pair<int, int> position) {
        if (position.first >= 0 && position.first <= 25 && position.second >= 0 && position.second <= 14) {
            ballPosition = position;
        } else {
            if (currentState == stateList.at(KICKOFF).get() || (currentState == stateList.at(PLAYERTURN).get() && getNbRepetition() == getNbRepetitionMax())) {
                currentTeam = (currentTeam == &teamA) ? &teamB : &teamA;
                unsigned long teamSize = getCurrentTeam()->getPlayableCharacter().size();
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<unsigned long> dis(0, teamSize - 1);
                unsigned long i = dis(gen);

                ballPosition = getCurrentTeam()->getPlayableCharacter()[i]->getPosition();
                ballIsHold = true;
                getCurrentTeam()->getPlayableCharacter()[i]->setHasBall(true);
            }
        }
    }

    int BloodBowlGame::getTurnCounter() const {
        return turnCounter;
    }

    void BloodBowlGame::setTurnCounter(int newTurnCount) {
        turnCounter = newTurnCount;
    }

    int BloodBowlGame::getNbRepetition() const
    {
        return nb_repetition;
    }

    int BloodBowlGame::getNbRepetitionMax() const
    {
        return nb_repetition_max;
    }

    void BloodBowlGame::setNbRepetition(int nb_repetitionValue)
    {
        nb_repetition = nb_repetitionValue;
    }

    std::pair<int, int> BloodBowlGame::getBallPosition() const {
        return ballPosition;
    }

    std::ostream& operator<<(std::ostream& os, const BloodBowlGame& game) {
        os << "\n=== GAME STATE ===\n";
        os << "Current State: " << stateToString(game)<< "\n";
        os << "Turn Counter: " << game.getTurnCounter() << "\n";
        os << "Current Team: ";
        if (game.getCurrentTeam())
            os << game.getCurrentTeam()->getTeamId();
        else
            os << "None";
        os << "\n";
        os << "Score - Team A: " << game.getTeamA().getScore() << " | Team B: " << game.getTeamB().getScore() << "\n";
        renderBoardAscii(os, game);
        os << "==================\n";
        return os;
    }
}
