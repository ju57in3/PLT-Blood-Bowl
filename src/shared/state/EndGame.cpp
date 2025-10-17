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
            for (auto character : game->getTeamA().getCharacters()){
                character.setStatus(bench);
            }
            for (auto character : game->getTeamB().getCharacters()){
                character.setStatus(bench);
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
