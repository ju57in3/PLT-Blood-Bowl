#include <iostream>
#include <string>
#include <memory>

// The following lines are here to check that SFML is installed and working
#include <unistd.h>
#include <SFML/Graphics.hpp>

#include "utility/Constants.h"

void testSFML() {
    sf::Texture texture;
}
// end of test SFML

#include <client.h>
#include <state.h>
#include <render.h>
#include <engine.h>
#include <ai/RandomAI.h>
#include <ai/HeuristicAI.h>

using namespace std;
using namespace client;
using namespace state;

int main(int argc, char* argv[]) {
    cout << "=== BLOOD BOWL GAME TEST ===\n\n";

    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto hum1 = std::make_shared<Character>(1,"Ligne1", "human_blitzer", 6, 4, 3, 8);
    teamA.addCharacter(hum1);
    hum1->setPosition({12, 6});
    hum1->setStatus(playable);

    auto hum2 = std::make_shared<Character>(2,"Ligne2", "human_blitzer", 6, 12, 3, 8);
    teamA.addCharacter(hum2);
    hum2->setPosition({12, 7});
    hum2->setStatus(playable);

    auto hum3 = std::make_shared<Character>(3,"Ligne3", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum3);
    hum3->setPosition({12, 8});
    hum3->setStatus(playable);

    auto hum4 = std::make_shared<Character>(4,"Bottom1", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum4);
    hum4->setPosition({10, 2});
    hum4->setStatus(playable);

    auto hum5 = std::make_shared<Character>(5,"Bottom2", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum5);
    hum5->setPosition({10, 3});
    hum5->setStatus(playable);

    auto hum6 = std::make_shared<Character>(6,"Top1", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum6);
    hum6->setPosition({10, 12});
    hum6->setStatus(playable);

    auto hum7 = std::make_shared<Character>(7,"Top2", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum7);
    hum7->setPosition({10, 13});
    hum7->setStatus(playable);

    auto hum8 = std::make_shared<Character>(8, "Behind", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum8);
    hum8->setPosition({5, 5});
    hum8->setStatus(playable);

    auto hum9 = std::make_shared<Character>(9, "Behind", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum9);
    hum9->setPosition({5, 6});
    hum9->setStatus(playable);

    auto hum10 = std::make_shared<Character>(10, "Behind", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum10);
    hum10->setPosition({5, 8});
    hum10->setStatus(playable);

    auto hum11 = std::make_shared<Character>(11, "Behind", "human_blitzer", 6, 3, 3, 8);
    teamA.addCharacter(hum11);
    hum11->setPosition({5, 9});
    hum11->setStatus(playable);


    auto orc1 = std::make_shared<Character>(1, "Ligne1", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc1);
    orc1->setPosition({13, 6});
    orc1->setStatus(playable);

    auto orc2 = std::make_shared<Character>(2, "Ligne2", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc2);
    orc2->setPosition({13, 7});
    orc2->setStatus(playable);

    auto orc3 = std::make_shared<Character>(3, "Ligne3", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc3);
    orc3->setPosition({13, 8});
    orc3->setStatus(playable);

    auto orc4 = std::make_shared<Character>(4, "Bottom1", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc4);
    orc4->setPosition({13, 2});
    orc4->setStatus(playable);

    auto orc5 = std::make_shared<Character>(5, "Bottom2", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc5);
    orc5->setPosition({13, 3});
    orc5->setStatus(playable);

    auto orc6 = std::make_shared<Character>(6, "Top1", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc6);
    orc6->setPosition({15, 12});
    orc6->setStatus(playable);

    auto orc7 = std::make_shared<Character>(7, "Top2", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc7);
    orc7->setPosition({15, 13});
    orc7->setStatus(playable);

    auto orc8 = std::make_shared<Character>(8, "Behind", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc8);
    orc8->setPosition({20, 5});
    orc8->setStatus(playable);

    auto orc9 = std::make_shared<Character>(9, "Behind", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc9);
    orc9->setPosition({20, 6});
    orc9->setStatus(playable);

    auto orc10 = std::make_shared<Character>(10, "Behind", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc10);
    orc10->setPosition({20, 8});
    orc10->setStatus(playable);

    auto orc11 = std::make_shared<Character>(11, "Behind", "orc_blitzer", 6, 3, 3, 8);
    teamB.addCharacter(orc11);
    orc11->setPosition({20, 9});
    orc11->setStatus(playable);

    // Initialize game
    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);

    // Create Engine
    engine::Engine eng(gamePtr);

    //ai::RandomAI randomAI(eng, gamePtr, 2);
    ai::HeuristicAI heuristicAI(eng, gamePtr, 2);

    heuristicAI.placePlayers();

    bool aiPlayedThisTurn = false;

    sf::RenderWindow window(sf::VideoMode(utility::Constants::WINDOW_WIDTH,utility::Constants::WINDOW_HEIGHT),"BloodBowl");
    render::Scene scene(render::SceneId::MENU, gamePtr);

    client::InputHandler inputHandler(gamePtr, &eng);

    // Create and add commands
    cout << "\n=== TESTING COMMANDS ===\n";
    cout << "Position initiale de hum1: (" << hum1->getPosition().first << ", " << hum1->getPosition().second << ")\n";

    cout << "\n=== INTERACTIVE MODE ===\n";
    cout << "Controls:\n";
    cout << "  - Click on a character to select it\n";
    cout << "  - Right click to Move/Block/Pass\n";
    cout << "  - L to list team A\n";
    cout << "  - Q in main window to quit\n\n";


    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            inputHandler.handleEvent(event, &window, scene.getDiceOptionBounds());

            if (event.type == sf::Event::KeyReleased) {
                switch (event.key.code) {
                    case sf::Keyboard::Q:
                        window.close();
                        break;

                    case sf::Keyboard::L:
                        cout << "Ball position : "<< gamePtr->getBallPosition().first << "," << gamePtr->getBallPosition().second << "\n";
                        break;

                    default:
                        break;
                }
            }
        }

        if (gamePtr && gamePtr->getCurrentState()) {
            gamePtr->getCurrentState()->update();
        }


        if (gamePtr) {
            state::Team* currentTeam = gamePtr->getCurrentTeam();

            if (currentTeam && currentTeam->getTeamId() == 2) {
                if (!aiPlayedThisTurn) {
                    //randomAI.runAI();
                    heuristicAI.runAI();
                    aiPlayedThisTurn = true;
                }
            } else {
                aiPlayedThisTurn = false;
            }
        }

        eng.executeCommand();

        std::vector<std::pair<int,int>> playablePositions;
        if (gamePtr) {
            for (const auto& c : gamePtr->getTeamA().getCharacters()) {
                if (c->getStatus() == playable) playablePositions.push_back(c->getPosition());
            }
            for (const auto& c : gamePtr->getTeamB().getCharacters()) {
                if (c->getStatus() == playable) playablePositions.push_back(c->getPosition());
            }
        }

        scene.drawScene(window, inputHandler.getSelectedCharacter(), inputHandler.getPreviewPosition(), inputHandler.hasPreviewPosition(), inputHandler.isPreviewLegal(), inputHandler.getPendingBlockDiceOptions(), inputHandler.hasPendingBlock(), playablePositions, inputHandler.getMovePath(), "", -1);
    }
    return 0;
}