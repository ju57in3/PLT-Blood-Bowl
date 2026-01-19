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
        : game(std::move(game)), currentAI(nullptr), secondAI(nullptr), lastAITeamTurn(-1)
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

    void Engine::setSecondAI(std::unique_ptr<ai::AI> ai)
    {
        secondAI = std::move(ai);
        if (secondAI) {
            std::cout << "[ENGINE] Second AI configured for team " << secondAI->teamId << "\n";
        }
    }

    ai::AI* Engine::getAI() const
    {
        return currentAI.get();
    }

    ai::AI* Engine::getSecondAI() const
    {
        return secondAI.get();
    }

    void Engine::runAITurnIfNeeded()
    {
        if (!game) {
            return;
        }

        // Vérifier si c'est le tour de l'équipe contrôlée par l'IA
        auto currentTeam = game->getCurrentTeam();
        if (!currentTeam) {
            return;
        }

        int currentTeamId = currentTeam->getTeamId();

        // Essayer avec la première IA
        if (currentAI && currentTeamId == currentAI->teamId && lastAITeamTurn != currentTeamId) {
            std::cout << "[ENGINE] Running AI for team " << currentAI->teamId << "\n";
            currentAI->runAI();
            lastAITeamTurn = currentTeamId; // Marquer que l'IA a joué ce tour
        }
        // Essayer avec la deuxième IA
        else if (secondAI && currentTeamId == secondAI->teamId && lastAITeamTurn != currentTeamId) {
            std::cout << "[ENGINE] Running second AI for team " << secondAI->teamId << "\n";
            secondAI->runAI();
            lastAITeamTurn = currentTeamId; // Marquer que l'IA a joué ce tour
        }

        // Réinitialiser le flag si on a changé d'équipe
        if (lastAITeamTurn != -1 && currentTeamId != lastAITeamTurn) {
            lastAITeamTurn = -1;
        }
    }

    void Engine::runAISetupIfNeeded(state::Setup* setupState)
    {
        if (!setupState || !game) {
            return;
        }

        auto* currentTeam = game->getCurrentTeam();
        if (!currentTeam) {
            return;
        }

        int currentTeamId = currentTeam->getTeamId();
        ai::AI* activeAI = nullptr;

        // Déterminer quelle IA est active
        if (currentAI && currentAI->teamId == currentTeamId) {
            activeAI = currentAI.get();
        } else if (secondAI && secondAI->teamId == currentTeamId) {
            activeAI = secondAI.get();
        }

        // Si une IA contrôle l'équipe actuelle ET n'a pas encore fini son setup
        if (activeAI && !setupState->isTeamSetupDone(currentTeamId)) {
            std::cout << "[ENGINE] AI placing players for team " << currentTeamId << "\n";

            // L'IA place ses joueurs
            activeAI->placePlayers();

            // Valider le setup
            if (setupState->isValidSetup(*currentTeam)) {
                std::cout << "[ENGINE] AI setup valid, ending setup for team " << currentTeamId << "\n";
                setupState->endSetup();
            } else {
                std::cerr << "[ENGINE] AI setup invalid for team " << currentTeamId << "!\n";
            }
        }
    }
} // namespace engine
