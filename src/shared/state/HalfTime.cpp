#include "HalfTime.h"

#include "BloodBowlGame.h"
#include "Setup.h"

namespace state {
    HalfTime::HalfTime(BloodBowlGame *game) : AbstractState(game) {
    }

    void HalfTime::update() {
        checkKO(game);
        game->setCurrentState(game->getStateList().at(SETUP).get());
    }

    void HalfTime::checkKO (BloodBowlGame* game) {
        for (Character character : game->getTeamA().getCharacters()) {
            if (character.getStatus() == ko) {
                int diceRoll = rand() % 2;
                if (diceRoll == 1) {
                    character.setStatus(bench);
                }
            }
        }
        for (Character character : game->getTeamB().getCharacters()) {
            if (character.getStatus() == ko) {
                int diceRoll = rand() % 2;
                if (diceRoll == 1) {
                    character.setStatus(bench);
                }
            }
        }
    }

    HalfTime::~HalfTime() {
    }

}
