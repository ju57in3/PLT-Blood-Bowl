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

    std::pair<int,int> BloodBowlGame::getBallPosiiton() {
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

    std::vector<AbstractState> BloodBowlGame::getStateList() {
        return statelist;
    }

    int BloodBowlGame::getTurnCounter() {
        return turnCounter;
    }


}
