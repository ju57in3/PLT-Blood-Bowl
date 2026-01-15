#include <iostream>
#include <string>
#include <memory>

// The following line is here to check that SFML is installed and working
#include <SFML/Graphics.hpp>

#include "utility/Constants.h"
#include "client.h"
#include "state.h"
#include "render.h"
#include "engine.h"

#include "screen/SceneManager.h"
#include "screen/ResourceManager.h"
#include "screen/HomeScreen.h"
#include "screen/TeamCreationScreen.h"
#include "screen/MatchCreationScreen.h"
#include "screen/GameScreen.h"
#include "screen/EndGameScreen.h"

using namespace std;
using namespace client;
using namespace state;

// Fonction pour créer des équipes par défaut pour les tests
void createDefaultTeams() {
    auto& teamManager = TeamManager::getInstance();

    // Créer équipe Humans par défaut
    auto teamA = std::make_unique<Team>(1, "Humans Default", 3);

    auto hum1 = std::make_shared<Character>(1,"Ligne1", "human_blitzer", 6, 4, 3, 8);
    teamA->addCharacter(hum1);
    hum1->setStatus(bench);

    auto hum2 = std::make_shared<Character>(2,"Ligne2", "human_blitzer", 6, 12, 3, 8);
    teamA->addCharacter(hum2);
    hum2->setStatus(bench);

    auto hum3 = std::make_shared<Character>(3,"Ligne3", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum3);
    hum3->setStatus(bench);

    auto hum4 = std::make_shared<Character>(4,"Bottom1", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum4);
    hum4->setStatus(bench);

    auto hum5 = std::make_shared<Character>(5,"Bottom2", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum5);
    hum5->setStatus(bench);

    auto hum6 = std::make_shared<Character>(6,"Top1", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum6);
    hum6->setStatus(bench);

    auto hum7 = std::make_shared<Character>(7,"Top2", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum7);
    hum7->setStatus(bench);

    auto hum8 = std::make_shared<Character>(8, "Behind", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum8);
    hum8->setStatus(bench);

    auto hum9 = std::make_shared<Character>(9, "Behind", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum9);
    hum9->setStatus(bench);

    auto hum10 = std::make_shared<Character>(10, "Behind", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum10);
    hum10->setStatus(bench);

    auto hum11 = std::make_shared<Character>(11, "Behind", "human_blitzer", 6, 3, 3, 8);
    teamA->addCharacter(hum11);
    hum11->setStatus(bench);

    teamManager.saveTeam(std::move(teamA));

    // Créer équipe Orcs par défaut
    auto teamB = std::make_unique<Team>(2, "Orcs Default", 2);

    auto orc1 = std::make_shared<Character>(1, "Ligne1", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc1);
    orc1->setStatus(bench);

    auto orc2 = std::make_shared<Character>(2, "Ligne2", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc2);
    orc2->setStatus(bench);

    auto orc3 = std::make_shared<Character>(3, "Ligne3", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc3);
    orc3->setStatus(bench);

    auto orc4 = std::make_shared<Character>(4, "Bottom1", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc4);
    orc4->setStatus(bench);

    auto orc5 = std::make_shared<Character>(5, "Bottom2", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc5);
    orc5->setStatus(bench);

    auto orc6 = std::make_shared<Character>(6, "Top1", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc6);
    orc6->setStatus(bench);

    auto orc7 = std::make_shared<Character>(7, "Top2", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc7);
    orc7->setStatus(bench);

    auto orc8 = std::make_shared<Character>(8, "Behind", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc8);
    orc8->setStatus(bench);

    auto orc9 = std::make_shared<Character>(9, "Behind", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc9);
    orc9->setStatus(bench);

    auto orc10 = std::make_shared<Character>(10, "Behind", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc10);
    orc10->setStatus(bench);

    auto orc11 = std::make_shared<Character>(11, "Behind", "orc_blitzer", 6, 3, 3, 8);
    teamB->addCharacter(orc11);
    orc11->setStatus(bench);

    teamManager.saveTeam(std::move(teamB));

    cout << "Équipes par défaut créées: " << teamManager.getTeamCount() << " équipes" << endl;
}

int main(int argc, char* argv[]) {
    cout << "=== BLOOD BOWL GAME TEST ===\n";

    // Créer des équipes par défaut pour les tests
    createDefaultTeams();

    // Récupérer les équipes depuis le TeamManager pour créer le jeu initial
    auto& teamManager = TeamManager::getInstance();
    Team* teamA = teamManager.getTeamById(1);
    Team* teamB = teamManager.getTeamById(2);

    if (!teamA || !teamB) {
        cerr << "Erreur: impossible de charger les équipes par défaut" << endl;
        return 1;
    }

    // Initialize game avec les équipes du TeamManager
    auto gamePtr = std::make_shared<BloodBowlGame>(*teamA, *teamB);

    // Create Engine
    engine::Engine eng(gamePtr);

    sf::RenderWindow window(sf::VideoMode(utility::Constants::WINDOW_WIDTH,utility::Constants::WINDOW_HEIGHT),"BloodBowl");

    // NEW: screen manager + resources
    screen::ResourceManager resources("../res/");
    screen::SceneManager scenes(&window, &resources);
    scenes.setGame(gamePtr);
    scenes.setEngine(&eng);

    // register screens
    scenes.registerScreen(std::make_unique<screen::HomeScreen>());
    scenes.registerScreen(std::make_unique<screen::TeamCreationScreen>());
    scenes.registerScreen(std::make_unique<screen::MatchCreationScreen>());
    scenes.registerScreen(std::make_unique<screen::GameScreen>());
    scenes.registerScreen(std::make_unique<screen::EndGameScreen>());

    scenes.switchTo(render::SceneId::HOME);

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            scenes.handleEvent(event,window);

            if (event.type == sf::Event::KeyReleased) {
                switch (event.key.code) {
                    case sf::Keyboard::Q:
                        window.close();
                        break;

                    case sf::Keyboard::L:
                        cout << *teamA << endl;
                        cout << *teamB << endl;
                        break;

                    default:
                        break;
                }
            }
        }

        if (gamePtr && gamePtr->getCurrentState()) {
            gamePtr->getCurrentState()->update();
        }

        // scenes update/draw
        scenes.update();
        scenes.draw(window);
    }
    return 0;
}
