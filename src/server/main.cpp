#include "server/GameServer.h"
#include "state/TeamManager.h"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> running(true);

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    running = false;
}

int main(int argc, char* argv[]) {
    // Gérer les signaux pour un arrêt propre
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    uint16_t port = 8080;
    if (argc > 1) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    std::cout << "Blood Bowl Server" << std::endl;
    std::cout << "=================" << std::endl;

    // Charger les équipes disponibles
    auto& teamManager = state::TeamManager::getInstance();
    teamManager.loadFromDisk("teams.json");
    std::cout << "Loaded " << teamManager.getSavedTeams().size() << " teams" << std::endl;

    // Créer et démarrer le serveur
    server::GameServer server(port);
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;

    // Boucle principale
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    server.stop();
    std::cout << "Server stopped cleanly" << std::endl;

    return 0;
}

