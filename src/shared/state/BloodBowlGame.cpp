#include "BloodBowlGame.h"
#include "Setup.h"
#include "Kickoff.h"
#include "PlayerTurn.h"
#include "HalfTime.h"
#include "EndGame.h"


namespace  state {
    BloodBowlGame::BloodBowlGame(Team teamA, Team teamB)
        :
    teamA(teamA),
    teamB(teamB),
    currentTeam(coinToss()),
    turnCounter(0){
        statelist.push_back(Setup(this));
        statelist.push_back(Kickoff(this));
        statelist.push_back(PlayerTurn(this));
        statelist.push_back(HalfTime(this));
        statelist.push_back(EndGame(this));
        currentState =  &statelist.at(SETUP);
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

    int BloodBowlGame::getTurnCounter() {
        return turnCounter;
    }


}
