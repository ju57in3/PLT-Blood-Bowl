#include <iostream>

#include "HalfTime.h"
#include "BloodBowlGame.h"
#include "Setup.h"

constexpr std::pair<int,int> HORS_TAB = {-1, -1};

namespace state {
    HalfTime::HalfTime(BloodBowlGame *game) : AbstractState(game) {
    }

    void HalfTime::update() {
        checkKO(game);
        /*std::cout << "[HALFTIME] Resetting positions for second half setup.\n";

        auto resetTeamPositions = [](state::Team& team) {
            for (auto& c : team.getCharacters()) {
                if (!c) continue;

                if (c->getStatus() == state::playable || c->getStatus() == state::played) {
                    c->setPosition(HORS_TAB);
                    c->setStatus(state::playable);
                }
            }
        };
        resetTeamPositions(game->getTeamA());
        resetTeamPositions(game->getTeamB());
        game->setCurrentTeam(&game->getTeamA());*/
        game->setCurrentState(game->getStateList().at(SETUP).get());
    }

    void HalfTime::checkKO (BloodBowlGame* game) {
        for (auto& characterPtr : game->getTeamA().getCharacters()) {
            if (characterPtr != nullptr) {
                Character& character = *characterPtr;
                if (character.getStatus() == ko) {
                    int diceRoll = rand() % 2;
                    if (diceRoll == 1) {
                        character.setStatus(bench);
                    }
                }
            }
        }
        for (auto& characterPtr : game->getTeamB().getCharacters()) {
            if (characterPtr != nullptr) {
                Character& character = *characterPtr;
                if (character.getStatus() == ko) {
                    int diceRoll = rand() % 2;
                    if (diceRoll == 1) {
                        character.setStatus(bench);
                    }
                }
            }
        }
    }

    HalfTime::~HalfTime() {
    }

    std::string HalfTime::getName() const {
        return "HalfTime";
    }
}
