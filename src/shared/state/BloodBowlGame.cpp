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
        currentTeam = &this->teamA; // Par défaut, pointer sur teamA
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

    Team* BloodBowlGame::coinToss() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        int toss = dis(gen);
        if (toss == 0) {
            return &this->teamA;
        } else {
            return &this->teamB;
        }
    }

    // Const-qualified getters
    const Team& BloodBowlGame::getTeamA() const {
        return teamA;
    }

    const Team& BloodBowlGame::getTeamB() const {
        return teamB;
    }

    Team &BloodBowlGame::getTeamA() {
        return teamA;
    }

    Team &BloodBowlGame::getTeamB() {
        return teamB;
    }



    Team* BloodBowlGame::getCurrentTeam() const {
        return currentTeam;
    }

    void BloodBowlGame::setTurnCounter(int newTurnCount) {
        turnCounter = newTurnCount;
    }

    void BloodBowlGame::setCurrentTeam(Team* team) {
        currentTeam = team;
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

    void BloodBowlGame::setBallPosition(std::pair<int,int> position)
    {
        if (position.first>=0 && position.first<=25 && position.second>=0 && position.second<=14)
        {
            ballPosition = position;
            setNbRepetition(getNbRepetition() + 1);
        }
        else
        {
            if (currentState==stateList.at(KICKOFF).get() || (currentState==stateList.at(PLAYERTURN).get() && getNbRepetition()==getNbRepetitionMax()))
            {
                if (currentTeam == &this->teamA)
                {
                    currentTeam = &this->teamB;
                }
                else
                {
                    currentTeam = &this->teamA;
                }
                unsigned long teamSize = getCurrentTeam()->getPlayableCharacter().size();  //PlayableCharacters pour éviter de donner le ballon à un jour sur le banc
                unsigned long i = rand()%teamSize;

                ballPosition = getCurrentTeam()->getPlayableCharacter()[i]->getPosition();
                ballIsHold = true;
                getCurrentTeam()->getPlayableCharacter()[i]->setHasBall(true);
            }

            else if (currentState==stateList.at(PLAYERTURN).get())
            {
                std::pair<int,int> newPosition;
                if (position.second>=height)
                {
                    newPosition.second = height -1;
                }
                if (position.second < 0)
                {
                    newPosition.second = 0;
                }
                if (position.first>=width)
                {
                    newPosition.first = width -1;
                }
                if (position.first < 0)
                {
                    newPosition.first = 0;
                }

                int direction = rand()%3;
                int rebounds = rand()%6 + rand()%6;
                if (newPosition.second == 0)
                {
                    switch (direction)
                    {
                        case 0: //North-West
                            newPosition.first -= rebounds;
                            newPosition.second += rebounds;
                            break;
                        case 1: // North
                            newPosition.second += rebounds;
                            break;
                        case 2: //North-East
                            newPosition.first += rebounds;
                            newPosition.second += rebounds;
                            break;
                        default: ;
                    }
                }
                else if (newPosition.first == 0)
                {
                    switch (direction)
                    {
                        case 0: // North-East
                            newPosition.first += rebounds;
                            newPosition.second += rebounds;
                            break;
                        case 1: // East
                            newPosition.first += rebounds;
                            break;
                        case 2: // South-East
                            newPosition.first += rebounds;
                            newPosition.second -= rebounds;
                            break;
                        default: ;
                    }
                }
                else if (newPosition.first == 14)
                {
                    switch (direction)
                    {
                        case 0: // South-East
                            newPosition.first += rebounds;
                            newPosition.second -= rebounds;
                            break;
                        case 1: // South
                            newPosition.second += rebounds;
                            break;
                        case 2: // South-West
                            newPosition.first -= rebounds;
                            newPosition.second -= rebounds;
                            break;
                    default: ;
                        newPosition.first -= rebounds;
                    }
                }
                else if (newPosition.first == 25)
                {
                    switch (direction)
                    {
                        case 0: // South-West
                            newPosition.first -= rebounds;
                            newPosition.second -= rebounds;
                            break;
                        case 1: // West
                            newPosition.first -= rebounds;
                            break;
                        case 2: // North-West
                            newPosition.first -= rebounds;
                            newPosition.second += rebounds;
                            break;
                    default: ;
                    }
                }
                nb_repetition += 1;
                setBallPosition(newPosition);
            }
        }
    }

    int BloodBowlGame::getTurnCounter() const {
        return turnCounter;
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




    std::ostream& operator<<(std::ostream& os, const BloodBowlGame& game) {
        os << "\n=== GAME STATE ===\n";
        os << "Current State: " << (game.getCurrentState() ? typeid(*game.getCurrentState()).name() : "None") << "\n";
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
