//
// Created by justine on 28/11/2025.
//
#include "RandomAI.h"
#include "state/BloodBowlGame.h"
#include "engine/Command.h"

namespace ai {
    RandomAI::RandomAI(engine::Command *engine, std::shared_ptr<state::BloodBowlGame> game, int teamId) : AI(engine, std::move(game), teamId){
        std::random_device rd;
        rng = std::mt19937(rd());
    }
    RandomAI::~RandomAI() = default;

    bool RandomAI::runAI() {
        if (!engine || !game) {
            return false;
        }
        std::vector<std::shared_ptr<::engine::Command>> possibleCommands;

        if (possibleCommands.empty()) {
            return false;
        }

        auto cmd = chooseRandomCommand(possibleCommands);
        if (!cmd) {
            return false;
        }

        engine->executeCommand(*cmd);

        return true;
    }

    std::shared_ptr<engine::Command>
    RandomAI::chooseRandomCommand(const std::vector<std::shared_ptr<engine::Command>> &cmds) {
        if (cmds.empty()) {
            return nullptr;
        }
        std::uniform_int_distribution<std::size_t> dist(0, cmds.size() - 1);
        std::size_t index = dist(rng);
        return cmds[index];
    }

    const std::mt19937 &RandomAI::getRng() const {
        return rng;
    }

    void RandomAI::setRng(const std::mt19937 &rng) {
        RandomAI::rng = rng;
    }
}