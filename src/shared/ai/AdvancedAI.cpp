//
// Created by justine on 19/12/2025.
//

#include "AdvancedAI.h"

#include <algorithm>
#include <iostream>
#include <limits>

#include <engine/Block.h>
#include <engine/Move.h>
#include <utility/Constants.h>
#include <utility/GameUtils.h>
#include <state/BloodBowlGame.h>
#include <state/Character.h>

namespace ai {
    AdvancedAI::AdvancedAI(engine::Engine &engine, std::shared_ptr<state::BloodBowlGame> &game, int teamId) : AI(engine, game, teamId), successfulActions(), probabilityMap(utility::Constants::BOARD_WIDTH * utility::Constants::BOARD_HEIGHT, 0.0f), targetMode(false){

    }

    AdvancedAI::~AdvancedAI() = default;

    int AdvancedAI::indexOf(int x, int y, int width) {
        return y * width + x;
    }

    int AdvancedAI::sign(int v) {
        if (v < 0) return -1;
        if (v > 0) return 1;
        return 0;
    }

    std::pair<int,int> AdvancedAI::clampToBoard(std::pair<int,int> p) const {
        int x = std::clamp(p.first, 0, utility::Constants::BOARD_WIDTH - 1);
        int y = std::clamp(p.second, 0, utility::Constants::BOARD_HEIGHT - 1);
        return {x, y};
    }

    std::pair<int,int> AdvancedAI::stepToward(std::pair<int,int> from, std::pair<int,int> to) const {
        int dx = sign(to.first - from.first);
        int dy = sign(to.second - from.second);
        return clampToBoard({from.first + dx, from.second + dy});
    }

    std::shared_ptr<state::Character> AdvancedAI::findBallCarrier(state::Team& team) const {
        for (const auto& c : team.getCharacters()) {
            if (c && c->hasBall()) {
                return c;
            }
        }
        return nullptr;
    }

    std::shared_ptr<state::Character> AdvancedAI::pickBestPlayable(state::Team& team) const {
        for (const auto& c : myTeam.getCharacters()) {
            if (c && c->getStatus() == state::playable) {
                return c;
            }
        }
        return nullptr;
    }

    void AdvancedAI::updateProbabilities() {
        std::fill(probabilityMap.begin(), probabilityMap.end(), 0.0f);
        state::Team* myTeam = nullptr;
        state::Team* oppTeam = nullptr;

        if (game->getTeamA().getTeamId() == teamId) {
            myTeam = &game->getTeamA();
            oppTeam = &game->getTeamB();
        } else {
            myTeam = &game->getTeamB();
            oppTeam = &game->getTeamA();
        }

        if (!myTeam || !oppTeam) return;

        auto ballPos = game->getBallPosition();
        defineSpace();
        if (auto oppCarrier = findBallCarrier(*oppTeam)) {
            defineSpace();
        }

        if (targetMode && !successfulActions.empty()) {
            for (auto p : successfulActions) {
                defineSpace();
            }
        }
    }

    bool AdvancedAI::actionSucceeded() {
        auto ballPos = game->getBallPosition();

        state::Team* myTeam = (game->getTeamA().getTeamId() == teamId) ? &game->getTeamA() : &game->getTeamB();
        state::Team* oppTeam = (myTeam == &game->getTeamA()) ? &game->getTeamB() : &game->getTeamA();
        if (!myTeam || !oppTeam) return false;

        if (auto oppCarrier = findBallCarrier(*oppTeam)) {
            successfulActions.push_back(oppCarrier->getPosition());
            return true;
        }
        return false;
    }

    bool AdvancedAI::targetDestroyed() {
        if (successfulActions.empty()) return true;

    }
}
