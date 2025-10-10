#include "BloodBowlGame.h"

namespace  state {
    BloodBowlGame::BloodBowlGame(const Board &board, Team teamA, Team teamB, const int turnCounter, Team currentTeam)
        : board(board),
    teamA(teamA),
    teamB(teamB),
    currentTeam(coinToss()),
    turnCounter(turnCounter){
        currentState = nullptr;
    }

    AbstractState BloodBowlGame::getCurrentState() {
        return currentState;
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


}