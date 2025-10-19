#include "EndGame.h"
#include "Team.h"
#include "BloodBowlGame.h"

namespace state {
    EndGame::EndGame(BloodBowlGame* game):AbstractState(game) {
        restart = false;
    }

    void EndGame::update()
    {
        if (restart == true)
        {
            for (auto& characterPtr : game->getTeamA().getCharacters()){
                if (characterPtr != nullptr){
                    characterPtr->setStatus(bench);
                }
            }
            for (auto& characterPtr : game->getTeamB().getCharacters()){
                if (characterPtr != nullptr){
                    characterPtr->setStatus(bench);
                }
            }
            new BloodBowlGame(game->getTeamA(), game->getTeamB());
        }
    }

    EndGame::~EndGame() {
    }

    bool EndGame::getRestart() const {
        return restart;
    }

    void EndGame::setRestart(bool restartValue) {
        this->restart = restartValue;
    }


}
