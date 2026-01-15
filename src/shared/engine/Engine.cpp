//
// Created by guilhem on 13/11/2025.
//
#include "Engine.h"

#include <iostream>
#include <memory>
#include <ostream>
#include <utility>
#include "state/PlayerTurn.h"
#include "state/Setup.h"
#include "ai/AI.h"

namespace engine {

    Engine::Engine(std::shared_ptr<state::BloodBowlGame> game)
        : game(std::move(game)), currentAI(nullptr), lastAITeamTurn(-1)
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

    void Engine::setAI(std::unique_ptr<ai::AI> ai)
    {
        currentAI = std::move(ai);
        if (currentAI) {
            std::cout << "[ENGINE] AI configured for team " << currentAI->teamId << "\n";
        }
    }

    ai::AI* Engine::getAI() const
    {
        return currentAI.get();
    }

    void Engine::runAITurnIfNeeded()
    {
        if (!currentAI || !game) {
            return;
        }

        // Vérifier si c'est le tour de l'équipe contrôlée par l'IA
        auto currentTeam = game->getCurrentTeam();
        if (!currentTeam) {
            return;
        }

        int currentTeamId = currentTeam->getTeamId();

        // Si c'est le tour de l'IA ET qu'elle n'a pas déjà joué ce tour
        if (currentTeamId == currentAI->teamId && lastAITeamTurn != currentTeamId) {
            std::cout << "[ENGINE] Running AI for team " << currentAI->teamId << "\n";
            currentAI->runAI();
            lastAITeamTurn = currentTeamId; // Marquer que l'IA a joué ce tour
        }

        // Réinitialiser le flag si c'est le tour de l'autre équipe
        if (currentTeamId != currentAI->teamId) {
            lastAITeamTurn = -1;
        }
    }

    void Engine::runAISetupIfNeeded(state::Setup* setupState)
    {
        if (!setupState || !currentAI || !game) {
            return;
        }

        auto* currentTeam = game->getCurrentTeam();
        if (!currentTeam) {
            return;
        }

        // Si l'IA contrôle l'équipe actuelle ET n'a pas encore fini son setup
        if (currentAI->teamId == currentTeam->getTeamId() &&
            !setupState->isTeamSetupDone(currentTeam->getTeamId())) {

            std::cout << "[ENGINE] AI placing players for team " << currentTeam->getTeamId() << "\n";

            // L'IA place ses joueurs
            currentAI->placePlayers();

            // Valider le setup
            if (setupState->isValidSetup(*currentTeam)) {
                std::cout << "[ENGINE] AI setup valid, ending setup for team " << currentTeam->getTeamId() << "\n";
                setupState->endSetup();
            } else {
                std::cerr << "[ENGINE] AI setup invalid for team " << currentTeam->getTeamId() << "!\n";
            }
        }
    }
} // namespace engine
