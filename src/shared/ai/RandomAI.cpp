#include "RandomAI.h"
#include <cstdlib>
#include <ctime>
#include <utility>
#include <engine/Block.h>
#include <engine/Move.h>
#include <utility/Constants.h>
#include <utility/GameUtils.h>

namespace ai {

    RandomAI::RandomAI(engine::Engine* engine, std::shared_ptr<state::BloodBowlGame> game, int teamId)
    : AI(engine, std::move(game), teamId) {
    }


    RandomAI::~RandomAI() {}


    bool RandomAI::runAI() {

        std::uniform_int_distribution<int> d2(0,1);

        state::Team *aiTeam = nullptr;
        state::Team *opponentTeam = nullptr;
        if (game->getTeamA().getTeamId() == teamId) {
            aiTeam = &game->getTeamA();;
            opponentTeam = &game->getTeamB();
        } else {
            aiTeam = &game->getTeamB();
            opponentTeam = &game->getTeamA();
        }

        for (auto character : aiTeam->getCharacters()) {
            bool playCharacter = d2(utility::GameUtils::getRNG());
            if (!playCharacter || character->getStatus() != state::playable) {
               break; // skip Character
            }

            std::uniform_int_distribution<int> dAction(0,2);
            int action = dAction(utility::GameUtils::getRNG()); // 0: Move, 1: Block, 2: Pass
            switch (action) {
                case 0: {
                    std::uniform_int_distribution<int> dMove(1,character->getMovement());
                    int maxMove = dMove(utility::GameUtils::getRNG());
                    for (int i = 0; i < maxMove; i++) {
                        std::pair<int,int> newPosition = utility::GameUtils::scatterOnce(character->getPosition());
                        auto moveStepCmd = std::make_unique<engine::Move>(character, newPosition);
                        this->engine->addCommand(std::move(moveStepCmd));
                    }
                    break;
                }
                case 1: {
                    auto blockableCharacters = utility::GameUtils::blockableCharacters(character,*opponentTeam);
                    if (blockableCharacters.empty()) {
                        break;
                    }
                    std::uniform_int_distribution<size_t> dblock(0,blockableCharacters.size()-1);
                    size_t blockIndex = dblock(utility::GameUtils::getRNG());
                    auto blockCmd =  std::make_unique<engine::Block>(character, blockableCharacters[blockIndex]);
                    this->engine->addCommand(std::move(blockCmd));
                    break;
                }
                case 2: {

                    break;
                }

                default:
                    break;
            }
        }

        return false; // EndTurn
    }

} // namespace ai
