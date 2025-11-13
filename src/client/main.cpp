#include <iostream>
#include <string>
#include <memory>

#include <render.h>

// The following lines are here to check that SFML is installed and working
#include <SFML/Graphics.hpp>

void testSFML() {
    sf::Texture texture;
}
// end of test SFML

#include <client.h>
#include <state.h>

using namespace std;
using namespace client;
using namespace state;

int main(int argc, char* argv[]) {
    cout << "=== BLOOD BOWL GAME TEST ===\n\n";

    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto hum1 = std::make_shared<Character>("Ligne1", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum1);
    hum1->setPosition({12, 6});
    hum1->setStatus(playable);

    auto hum2 = std::make_shared<Character>("Ligne2", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum2);
    hum2->setPosition({12, 7});
    hum2->setStatus(playable);

    auto hum3 = std::make_shared<Character>("Ligne3", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum3);
    hum3->setPosition({12, 8});
    hum3->setStatus(playable);

    auto hum4 = std::make_shared<Character>("Bottom1", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum4);
    hum4->setPosition({10, 2});
    hum4->setStatus(playable);

    auto hum5 = std::make_shared<Character>("Bottom2", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum5);
    hum5->setPosition({10, 3});
    hum5->setStatus(playable);

    auto hum6 = std::make_shared<Character>("Top1", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum6);
    hum6->setPosition({10, 12});
    hum6->setStatus(playable);

    auto hum7 = std::make_shared<Character>("Top2", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum7);
    hum7->setPosition({10, 13});
    hum7->setStatus(playable);

    auto hum8 = std::make_shared<Character>("Behind", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum8);
    hum8->setPosition({5, 5});
    hum8->setStatus(playable);

    auto hum9 = std::make_shared<Character>("Behind", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum9);
    hum9->setPosition({5, 6});
    hum9->setStatus(playable);

    auto hum10 = std::make_shared<Character>("Behind", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum10);
    hum10->setPosition({5, 8});
    hum10->setStatus(playable);

    auto hum11 = std::make_shared<Character>("Behind", "Human", 6, 3, 3, 8);
    teamA.addCharacter(hum11);
    hum11->setPosition({5, 9});
    hum11->setStatus(playable);


    auto orc1 = std::make_shared<Character>("Ligne1", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc1);
    orc1->setPosition({13, 6});
    orc1->setStatus(playable);

    auto orc2 = std::make_shared<Character>("Ligne2", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc2);
    orc2->setPosition({13, 7});
    orc2->setStatus(playable);

    auto orc3 = std::make_shared<Character>("Ligne3", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc3);
    orc3->setPosition({13, 8});
    orc3->setStatus(playable);

    auto orc4 = std::make_shared<Character>("Bottom1", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc4);
    orc4->setPosition({13, 2});
    orc4->setStatus(playable);

    auto orc5 = std::make_shared<Character>("Bottom2", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc5);
    orc5->setPosition({13, 3});
    orc5->setStatus(playable);

    auto orc6 = std::make_shared<Character>("Top1", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc6);
    orc6->setPosition({15, 12});
    orc6->setStatus(playable);

    auto orc7 = std::make_shared<Character>("Top2", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc7);
    orc7->setPosition({15, 13});
    orc7->setStatus(playable);

    auto orc8 = std::make_shared<Character>("Behind", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc8);
    orc8->setPosition({20, 5});
    orc8->setStatus(playable);

    auto orc9 = std::make_shared<Character>("Behind", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc9);
    orc9->setPosition({20, 6});
    orc9->setStatus(playable);

    auto orc10 = std::make_shared<Character>("Behind", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc10);
    orc10->setPosition({20, 8});
    orc10->setStatus(playable);

    auto orc11 = std::make_shared<Character>("Behind", "orc", 6, 3, 3, 8);
    teamB.addCharacter(orc11);
    orc11->setPosition({20, 9});
    orc11->setStatus(playable);

    // Initialize game
    auto gamePtr = std::make_shared<BloodBowlGame>(BloodBowlGame(teamA,teamB));

    cout << "Initial Teams:\n";
    cout << "Team A (Humans):\n" << teamA << "\n";
    cout << "Team B (Orcs):\n" << teamB << "\n";

    // testing render
    cout << "\n=== TESTING RENDER ===\n";
    render::Scene scene(render::SceneId::MENU, gamePtr);

    // init scene
    scene.init(render::SceneId::MENU, gamePtr);

    // main loop
    while (scene.getWindow()->isOpen()) {
        sf::Event event{};
        while (scene.getWindow()->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                scene.getWindow()->close();
            }
        }

        // Dessin de la scène
        scene.drawScene();
    }

    cout << "\nRender test completed successfully!\n";

    
    cout << "\nTest completed successfully!\n";
    return 0;
}