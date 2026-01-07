#include <iostream>
#include "RandomAI.h"
#include <cstdlib>
#include <ctime>
#include <utility>
#include <engine/Block.h>
#include <engine/Move.h>
#include <utility/Constants.h>
#include <utility/GameUtils.h>

namespace ai {

    RandomAI::RandomAI(engine::Engine& engine, const std::shared_ptr<state::BloodBowlGame>& game, int teamId)
    : AI(engine, game, teamId) {
    }

    RandomAI::~RandomAI() {}


    bool RandomAI::runAI() {
        std::cout << "\n ====== AI (Team " << teamId << ") is playing ======\n";

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

        if (!aiTeam || !opponentTeam) {
            std::cout << "[AI] Error: Teams not found!\n";
            return false; // EndTurn
        }

        for (const auto& character : aiTeam->getCharacters()) {
            if (!character) {
                std::cout << "[AI] Characters non existent, skip.\n";
                continue;
            }
            if (character->getStatus() != state::CharacterStatus::playable) {
                std::cout << "[AI] " << character->getName() << " is not playable, skip.\n";
                continue;
            }
            bool playCharacter = d2(utility::GameUtils::getRNG());
            std::cout << "[AI] Considering " << character->getName() << " at position (" << character->getPosition().first << "," << character->getPosition().second << ")\n";
            if (!playCharacter) {
                std::cout << "[AI] Decided not to play " << character->getName() << "\n";
                continue;
            }
            std::cout << "[AI] Decided to play " << character->getName() << "\n";

            std::uniform_int_distribution<int> dAction(0,2);
            int action = dAction(utility::GameUtils::getRNG()); // 0: Move, 1: Block, 2: Pass
            std::cout << "[AI] " << character->getName() << " action (" << action << ")\n";
            switch (action) {
                case 0: {
                    std::uniform_int_distribution<int> dMove(1,character->getMovement());
                    int maxMove = dMove(utility::GameUtils::getRNG());
                    std::cout << "[AI] " << character->getName() << " will move up to " << maxMove << " steps.\n";
                    auto currentPos = character->getPosition();
                    for (int i = 0; i < maxMove; i++) {
                        std::pair<int,int> newPosition = utility::GameUtils::scatterOnce(character->getPosition());
                        std::cout << "      Step " << i+1
                                  << " : (" << currentPos.first << "," << currentPos.second << ") -> (" << newPosition.first << "," << newPosition.second << ")\n";
                        currentPos = newPosition;
                        auto moveStepCmd = std::make_unique<engine::Move>(character, newPosition);
                        this->engine.addCommand(std::move(moveStepCmd));
                    }
                    break;
                }
                case 1: {
                    auto blockableCharacters = utility::GameUtils::blockableCharacters(character,*opponentTeam);
                    if (blockableCharacters.empty()) {
                        std::cout << "      -> No blockable characters for " << character->getName() << ", skipping block action.\n";
                        break;
                    }
                    std::uniform_int_distribution<size_t> dblock(0,blockableCharacters.size()-1);
                    size_t blockIndex = dblock(utility::GameUtils::getRNG());
                    auto blockCmd =  std::make_unique<engine::Block>(character, blockableCharacters[blockIndex]);
                    this->engine.addCommand(std::move(blockCmd));
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
