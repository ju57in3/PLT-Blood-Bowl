//
// Created by guilhem on 13/11/2025.
//
#include "Engine.h"

#include <iostream>
#include <memory>
#include <ostream>
#include <utility>
#include "state/PlayerTurn.h"

namespace engine {

    Engine::Engine(std::shared_ptr<state::BloodBowlGame> game)
        : game(std::move(game))
    {
    }

    Engine::~Engine() = default;

    std::shared_ptr<state::BloodBowlGame> Engine::getGameState()
    {
        return game;
    }

    void Engine::setGameState(std::shared_ptr<state::BloodBowlGame> newGameState)
    {
        game = std::move(newGameState);
    }

    void Engine::executeCommand()
    {
        if (commandHistory.empty()) {
            std::cout << "Command history empty" << std::endl;
            return;
        }
        if (!game) {
            return;
        }

        auto cmd = std::move(commandHistory.front());
        commandHistory.pop_front();
        if (cmd) {
            cmd->execute(game);
        }
    }

    void Engine::addCommand(std::unique_ptr<Command> cmd_ptr)
    {
        if (!cmd_ptr) {
            return;
        }
        commandHistory.push_back(std::move(cmd_ptr));
    }
} // namespace engine
