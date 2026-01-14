//
// Created by justine on 12/12/2025.
//
#include "HeuristicAI.h"

#include <iostream>
#include <limits>
#include <random>
#include <thread>
#include <chrono>
#include <engine/Block.h>
#include <engine/Move.h>
#include <engine/EndTurn.h>
#include <utility/GameUtils.h>
#include <utility/Constants.h>
#include <state/PlayerTurn.h>

namespace ai {
    HeuristicAI::HeuristicAI(engine::Engine& engine, const std::shared_ptr<state::BloodBowlGame>& game, int teamId) : AI(engine, game, teamId){

    }

    HeuristicAI::~HeuristicAI() = default;

    bool HeuristicAI::runAI() {
        state::Team* myTeam = nullptr;
        state::Team* opponentTeam = nullptr;
        if (game->getTeamA().getTeamId() == teamId) {
            myTeam = &game->getTeamA();
            opponentTeam = &game->getTeamB();
        } else {
            myTeam = &game->getTeamB();
            opponentTeam = &game->getTeamA();
        }

        if (!myTeam || !opponentTeam) return false;

        std::cout << "\n[HEURISTIC AI] Team " << teamId << " is playing.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause avant de commencer

        auto ballPos = game->getBallPosition();

        // Parcourir tous les personnages jouables et leur assigner une action
        for (const auto& c : myTeam->getCharacters()) {
            if (!c || c->getStatus() != state::playable) continue;

            // Priorité 1: Bloquer un adversaire adjacent
            auto blockables = utility::GameUtils::blockableCharacters(c, *opponentTeam);
            if (!blockables.empty()) {
                std::cout << "[HEURISTIC AI] " << c->getName() << " blocking " << blockables.front()->getName() << "\n";
                auto blockCmd = std::make_unique<engine::Block>(c, blockables.front());
                engine.addCommand(std::move(blockCmd));
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause après l'action
                continue; // Passer au prochain personnage
            }

            // Priorité 2: Se déplacer vers la balle en utilisant la capacité de mouvement complète
            int maxMove = c->getMovement();
            std::pair<int,int> currentPos = c->getPosition();
            std::pair<int,int> targetPos = currentPos;

            // Calculer la direction vers la balle
            int dx = ballPos.first - currentPos.first;
            int dy = ballPos.second - currentPos.second;
            int distToBall = dx * dx + dy * dy;

            if (distToBall > 2) {
                // Si loin de la balle, aller vers elle
                std::cout << "[HEURISTIC AI] " << c->getName() << " moving towards ball\n";

                // Déplacer progressivement vers la balle
                for (int step = 0; step < maxMove; ++step) {
                    int nextX = targetPos.first;
                    int nextY = targetPos.second;

                    // Avancer vers la balle
                    if (nextX < ballPos.first) nextX++;
                    else if (nextX > ballPos.first) nextX--;

                    if (nextY < ballPos.second) nextY++;
                    else if (nextY > ballPos.second) nextY--;

                    // Vérifier les limites du terrain
                    if (nextX < 0 || nextX >= utility::Constants::BOARD_WIDTH ||
                        nextY < 0 || nextY >= utility::Constants::BOARD_HEIGHT) {
                        break;
                    }

                    targetPos = {nextX, nextY};

                    // Arrêter si on a atteint la balle
                    if (targetPos == ballPos) break;
                }

                auto moveCmd = std::make_unique<engine::Move>(c, targetPos);
                engine.addCommand(std::move(moveCmd));
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause après l'action
                continue; // Passer au prochain personnage
            }

            // Priorité 3: Mouvement aléatoire si proche de la balle
            std::uniform_int_distribution<int> dDist(1, maxMove);
            int moveDistance = dDist(utility::GameUtils::getRNG());

            for (int step = 0; step < moveDistance; ++step) {
                std::pair<int,int> nextPos = utility::GameUtils::scatterOnce(targetPos);

                if (nextPos.first < 0 || nextPos.first >= utility::Constants::BOARD_WIDTH ||
                    nextPos.second < 0 || nextPos.second >= utility::Constants::BOARD_HEIGHT) {
                    break;
                }

                targetPos = nextPos;
            }

            std::cout << "[HEURISTIC AI] " << c->getName() << " random move\n";
            auto moveCmd = std::make_unique<engine::Move>(c, targetPos);
            engine.addCommand(std::move(moveCmd));
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pause après l'action
        }

        std::cout << "[HEURISTIC AI] Turn complete, adding EndTurn command\n";

        // Ajouter la commande de fin de tour
        auto endTurnCmd = std::make_unique<engine::EndTurn>();
        engine.addCommand(std::move(endTurnCmd));

        return false;
    }
}
