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
        std::cout << "\n====== AI (Team " << teamId << ") is playing ======\n";

        std::uniform_int_distribution<int> d2(0,1);

        state::Team *aiTeam = nullptr;
        state::Team *opponentTeam = nullptr;
        if (game->getTeamA().getTeamId() == teamId) {
            aiTeam = &game->getTeamA();
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
                std::cout << "[AI] Character non existent, skip.\n";
                continue;
            }
            if (character->getStatus() != state::CharacterStatus::playable) {
                std::cout << "[AI] " << character->getName() << " is not playable, skip.\n";
                continue;
            }

            // 50% de chance de jouer ce personnage
            bool playCharacter = d2(utility::GameUtils::getRNG());
            std::cout << "[AI] Considering " << character->getName()
                      << " at position (" << character->getPosition().first
                      << "," << character->getPosition().second << ")\n";
            if (!playCharacter) {
                std::cout << "[AI] Decided not to play " << character->getName() << "\n";
                continue;
            }
            std::cout << "[AI] Decided to play " << character->getName() << "\n";

            // Choisir une action aléatoire : 0=Move, 1=Block, 2=Pass
            std::uniform_int_distribution<int> dAction(0,2);
            int action = dAction(utility::GameUtils::getRNG());
            std::cout << "[AI] " << character->getName() << " action (" << action << ")\n";

            switch (action) {
                case 0: { // MOVE
                    // Générer une position aléatoire adjacente
                    std::pair<int,int> newPosition = utility::GameUtils::scatterOnce(character->getPosition());
                    std::cout << "[AI] " << character->getName() << " moving from ("
                              << character->getPosition().first << ","
                              << character->getPosition().second << ") to ("
                              << newPosition.first << "," << newPosition.second << ")\n";

                    auto moveCmd = std::make_unique<engine::Move>(character, newPosition);
                    this->engine.addCommand(std::move(moveCmd));
                    return false; // Fin du tour après cette action
                }

                case 1: { // BLOCK
                    auto blockableCharacters = utility::GameUtils::blockableCharacters(character, *opponentTeam);
                    if (blockableCharacters.empty()) {
                        std::cout << "[AI] No blockable characters for " << character->getName()
                                  << ", trying next character.\n";
                        break; // Essayer le prochain personnage
                    }

                    // Bloquer un adversaire au hasard
                    std::uniform_int_distribution<size_t> dblock(0, blockableCharacters.size() - 1);
                    size_t blockIndex = dblock(utility::GameUtils::getRNG());

                    std::cout << "[AI] " << character->getName() << " blocking "
                              << blockableCharacters[blockIndex]->getName() << "\n";

                    auto blockCmd = std::make_unique<engine::Block>(character, blockableCharacters[blockIndex]);
                    this->engine.addCommand(std::move(blockCmd));
                    return false; // Fin du tour après cette action
                }

                case 2: { // PASS (non implémenté pour l'instant)
                    std::cout << "[AI] Pass action not implemented, trying next character.\n";
                    break;
                }

                default:
                    break;
            }
        }

        // Aucune action effectuée
        std::cout << "[AI] No action performed this turn.\n";
        return false; // EndTurn
    }

} // namespace ai
