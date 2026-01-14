#include <iostream>
#include "RandomAI.h"
#include <cstdlib>
#include <ctime>
#include <utility>
#include <thread>
#include <chrono>
#include <engine/Block.h>
#include <engine/Move.h>
#include <engine/EndTurn.h>
#include <utility/Constants.h>
#include <utility/GameUtils.h>
#include <state/PlayerTurn.h>

namespace ai {

    RandomAI::RandomAI(engine::Engine& engine, const std::shared_ptr<state::BloodBowlGame>& game, int teamId)
    : AI(engine, game, teamId) {
    }

    RandomAI::~RandomAI() {}


    bool RandomAI::runAI() {
        std::cout << "\n====== AI (Team " << teamId << ") is playing ======\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause avant de commencer

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
                    // Utiliser la capacité de mouvement complète du personnage
                    int maxMove = character->getMovement();

                    // Générer une distance aléatoire entre 1 et movementAllowance
                    std::uniform_int_distribution<int> dDist(1, maxMove);
                    int moveDistance = dDist(utility::GameUtils::getRNG());

                    std::pair<int,int> currentPos = character->getPosition();
                    std::pair<int,int> targetPos = currentPos;

                    // Déplacer le personnage step by step
                    for (int step = 0; step < moveDistance; ++step) {
                        std::pair<int,int> nextPos = utility::GameUtils::scatterOnce(targetPos);

                        // Vérifier si la case est dans les limites du terrain
                        if (nextPos.first < 0 || nextPos.first >= utility::Constants::BOARD_WIDTH ||
                            nextPos.second < 0 || nextPos.second >= utility::Constants::BOARD_HEIGHT) {
                            // Position invalide, arrêter le mouvement
                            break;
                        }

                        targetPos = nextPos;
                    }

                    std::cout << "[AI] " << character->getName() << " moving from ("
                              << currentPos.first << "," << currentPos.second << ") to ("
                              << targetPos.first << "," << targetPos.second
                              << ") [distance: " << moveDistance << " squares, max: " << maxMove << "]\n";

                    auto moveCmd = std::make_unique<engine::Move>(character, targetPos);
                    this->engine.addCommand(std::move(moveCmd));
                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause après l'action
                    break;
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
                    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause après l'action
                    break; // Continue avec les autres personnages
                }

                case 2: { // PASS (non implémenté pour l'instant)
                    std::cout << "[AI] Pass action not implemented, trying next character.\n";
                    break;
                }

                default:
                    break;
            }
        }

        std::cout << "[AI] Turn complete, adding EndTurn command\n";

        // Ajouter la commande de fin de tour
        auto endTurnCmd = std::make_unique<engine::EndTurn>();
        this->engine.addCommand(std::move(endTurnCmd));

        return false;
    }

} // namespace ai
