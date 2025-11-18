//
// Created by guilhem on 13/11/2025.
//
#include "Engine.h"
#include <memory>

namespace engine {

Engine::Engine()
    : gameState(nullptr)
{
}

Engine::~Engine() = default;

std::shared_ptr<state::BloodBowlGame> Engine::getGameState()
{
    return gameState;
}

void Engine::setGameState(std::shared_ptr<state::BloodBowlGame> newGameState)
{
    gameState = std::move(newGameState);
}

void Engine::executeCommand()
{
    if (commandHistory.empty()) {
        return;
    }
    if (!gameState) {
        return;
    }

    auto cmd = std::move(commandHistory.front());
    commandHistory.pop_front();
    if (cmd) {
        cmd->execute(gameState);
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
