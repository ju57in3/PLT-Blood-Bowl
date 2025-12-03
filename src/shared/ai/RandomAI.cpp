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

        // TODO : continue !

        // Randomly choose a player and an action
        // Placeholder logic for player and action selection
        int playerIndex = std::rand() % game->getPlayers().size();
        auto player = game->getPlayers()[playerIndex];

        int action = std::rand() % 3; // 0: Move, 1: Block, 2: Pass
        switch (action) {
            case 0:
                if (player.canMove()) {
                    player.move();
                }
                break;
            case 1:
                if (player.canBlock()) {
                    player.block();
                }
                break;
            case 2:
                if (player.canPass()) {
                    player.pass();
                }
                break;
            default:
                break;
        }

        return true; // Continue turn
    }

} // namespace ai
