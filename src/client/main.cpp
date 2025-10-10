#include <iostream>

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

    Team teamA(1, "Humans", 2);
    Team teamB(2, "Orcs", 1);

    Character c1("Hero", "Human", 6, 3, 3, 8);
    c1.setPosition({13,4});
    c1.setStatus(playable);

    Character c2("Benchwarmer", "Human", 4, 2, 2, 6);
    c2.setPosition({-1,-1});
    c2.setStatus(bench);

    Character c3("Rookie", "Orc", 5, 3, 2, 7);
    c3.setPosition({13,5});
    c3.setStatus(playable);

    teamA.addCharacter(c1);
    teamA.addCharacter(c2);

    teamB.addCharacter(c3);

    BloodBowlGame game(teamA, teamB);

    cout << "Team A:\n" << teamA << endl;
    cout << "Team B:\n" << teamB << endl;

    cout << "Coin toss winner team id: " << game.getCurrentTeam().getTeamId() << "\n";

    AbstractState* current = game.getCurrentState();
    if (current) {
        cout << "Current state exists (pointer): " << current << "\n";
    } else {
        cout << "No current state set." << endl;
    }

    for (int i = 0; i < 3; ++i) {
        cout << "--- update " << i << " ---\n";
        if (game.getCurrentState()) {
            game.getCurrentState()->update();
            cout << "Turn counter: " << game.getTurnCounter() << "\n";
        }
    }

    cout << "Final current state pointer: " << game.getCurrentState() << "\n";
    cout << "Final Team A score: " << game.getTeamA().getScore() << "\n";
    cout << "Final Team B score: " << game.getTeamB().getScore() << "\n";

    return 0;
}
