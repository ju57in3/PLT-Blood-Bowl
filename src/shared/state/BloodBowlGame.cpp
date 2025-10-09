#include "BloodBowlGame.h"

namespace  state {
    BloodBowlGame::BloodBowlGame (const Board &board, std::vector<Team> teams, const int turnCounter) :
    board(board), teams(teams), turnCounter(turnCounter) {
        this->currentState = nullptr;
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

    std::vector<Team> BloodBowlGame::getTeams() {
        return this->teams;
    }

}