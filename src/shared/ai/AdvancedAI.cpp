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

    std::pair<int,int> AdvancedAI::ste
}
