#include "RandomAI.h"
#include <cstdlib>
#include <ctime>
#include <utility>
#include <utility/Constants.h>

namespace ai {

    RandomAI::RandomAI(engine::Engine* engine, std::shared_ptr<state::BloodBowlGame> game, int teamId)
    : AI(engine, std::move(game), teamId) {
    }


    RandomAI::~RandomAI() {}

    bool RandomAI::runAI() {
        // Randomly decide whether to play a player or end turn

        std::uniform_int_distribution<int> d2(0,1);
        bool isEndTurn = d2(utility::GameUtils::getRNG());
        if (isEndTurn) {
            return false; // End turn
        }
        state::Team *aiTeam = nullptr;
        if (game->getTeamA().getTeamId() == teamId) {
            aiTeam = &game->getTeamA();;
        } else {
            aiTeam = &game->getTeamB();
        }

        for (auto character : aiTeam->getCharacters()) {
            bool playCharacter = d2(utility::GameUtils::getRNG());
            if (playCharacter) {
               break; // skip Character
            }

            std::uniform_int_distribution<int> daction(0,2);
            int action = daction(utility::GameUtils::getRNG()); // 0: Move, 1: Block, 2: Pass
            switch (action) {
                case 0:

                    break;
                case 1:

                    break;
                case 2:

                    break;
                default:
                    break;
            }
        }




        return true; // Continue turn
    }

} // namespace ai
