//
// Created by justine on 12/12/2025.
//
#include "HeuristicAI.h"

#include <iostream>
#include <limits>
#include <engine/Block.h>
#include <engine/Move.h>
#include <utility/GameUtils.h>

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

        std::cout << "\n [HEURISTIC AI] Team " << teamId << " is playing.\n";

        for (const auto& c : myTeam->getCharacters()) {
            if (!c || c->getStatus() != state::playable) continue;

            auto blockables = utility::GameUtils::blockableCharacters(c, *opponentTeam);

            if (!blockables.empty()) {
                std::cout << "[HEURISTIC AI] Blocking with " << c->getName() << "\n";
                auto blockCmd = std::make_unique<engine::Block>(c, blockables.front());
                engine.addCommand(std::move(blockCmd));
                return false;
            }
        }

        auto ballPos = game->getBallPosition();

        std::shared_ptr<state::Character> closest = nullptr;
        int bestDist = std::numeric_limits<int>::max();
        for (const auto& c : myTeam->getCharacters()) {
            if (!c || c->getStatus() != state::playable) continue;

            int dx = c->getPosition().first - ballPos.first;
            int dy = c->getPosition().second - ballPos.second;
            int dist = dx * dx + dy * dy;

            if (dist < bestDist) {
                bestDist = dist;
                closest = c;
            }

            if (closest) {
                std::cout << "[HEURISTIC AI] Moving towards ball with " << closest->getName() << "\n";
                auto nextPos = utility::GameUtils::scatterOnce(closest->getPosition());
                auto moveCmd = std::make_unique<engine::Move>(closest, ballPos);
                engine.addCommand(std::move(moveCmd));
                return false;
            }

            for (const auto& c : myTeam->getCharacters()) {
                if (!c || c->getStatus() != state::playable) continue;

                auto nextPos = utility::GameUtils::scatterOnce(c->getPosition());
                std::cout << "[HEURISTIC AI] Fallback move with " << c->getName() << "\n";
                auto moveCmd = std::make_unique<engine::Move>(c, nextPos);
                engine.addCommand(std::move(moveCmd));
                return false;
            }

            return false;
        }
    }
}
