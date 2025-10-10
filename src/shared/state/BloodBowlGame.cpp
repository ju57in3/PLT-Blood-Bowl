#include "BloodBowlGame.h"

namespace  state {
    BloodBowlGame::BloodBowlGame(Team teamA, Team teamB)
        :
    teamA(teamA),
    teamB(teamB),
    currentTeam(coinToss()),
    turnCounter(0){
        currentState = nullptr;
    }

    void BloodBowlGame::setCurrentState(AbstractState *state) {
        if (currentState != nullptr) {
            delete currentState;
        }
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

    Board BloodBowlGame::getBoard() {
        return board;
    }

    int BloodBowlGame::getTurnCounter() {
        return turnCounter;
    }


}