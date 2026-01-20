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
#include "screen/TeamManagementScreen.h"
#include "screen/MatchCreationScreen.h"
#include "screen/GameScreen.h"
#include "screen/EndGameScreen.h"
#include "screen/LoadGameScreen.h"
#include "screen/PauseScreen.h"

using namespace std;
using namespace client;
using namespace state;

int main(int argc, char* argv[]) {

    // Load teams from disk
    auto& teamManager = TeamManager::getInstance();
    teamManager.loadFromDisk("teams.json");

    // Create a placeholder game (will be replaced when starting a match)
    // For now, we just initialize with nullptr to avoid requiring teams at startup
    std::shared_ptr<BloodBowlGame> gamePtr = nullptr;

    sf::RenderWindow window(sf::VideoMode(utility::Constants::WINDOW_WIDTH,utility::Constants::WINDOW_HEIGHT),"BloodBowl", sf::Style::Fullscreen);

    // NEW: screen manager + resources
    screen::ResourceManager resources("../res/");
    screen::SceneManager scenes(&window, &resources);
    scenes.setGame(gamePtr);
    scenes.setEngine(nullptr); // Initialize with nullptr, will be set when match is created

    // register screens
    scenes.registerScreen(std::make_unique<screen::HomeScreen>());
    scenes.registerScreen(std::make_unique<screen::TeamCreationScreen>());
    scenes.registerScreen(std::make_unique<screen::TeamManagementScreen>());
    scenes.registerScreen(std::make_unique<screen::MatchCreationScreen>());
    scenes.registerScreen(std::make_unique<screen::GameScreen>());
    scenes.registerScreen(std::make_unique<screen::EndGameScreen>());
    scenes.registerScreen(std::make_unique<screen::LoadGameScreen>());
    scenes.registerScreen(std::make_unique<screen::PauseScreen>());

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

    // Save teams before exiting
    teamManager.saveToDisk("teams.json");

    return 0;
}
