#include "BloodBowlGame.h"

namespace  state {
    BloodBowlGame::BloodBowlGame (const Board &board, Team* teams, const int turnCounter) :
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

    Team BloodBowlGame::coinToss() {
        int toss = rand() % 2;
        if (toss == 0) {
            return this->teams[0];
        } else {
            return this->teams[1];
        }
    }




}