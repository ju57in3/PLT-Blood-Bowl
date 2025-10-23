#include <iostream>
#include <string>
#include <memory>

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

    teamA.addCharacter(std::make_unique<Character>("Ligne1", "Human", 6, 3, 3, 8));
    teamA.getCharacters().back()->setPosition({12, 6});
    teamA.getCharacters().back()->setStatus(playable);

    teamA.addCharacter(std::make_unique<Character>("Ligne2", "Human", 6, 3, 3, 8));
    teamA.getCharacters().back()->setPosition({12, 7});
    teamA.getCharacters().back()->setStatus(playable);

    teamA.addCharacter(std::make_unique<Character>("Ligne3", "Human", 6, 3, 3, 8));
    teamA.getCharacters().back()->setPosition({12, 8});
    teamA.getCharacters().back()->setStatus(playable);

    teamA.addCharacter(std::make_unique<Character>("Bottom1", "Human", 6, 3, 3, 8));
    teamA.getCharacters().back()->setPosition({10, 2});
    teamA.getCharacters().back()->setStatus(playable);

    teamA.addCharacter(std::make_unique<Character>("Bottom2", "Human", 6, 3, 3, 8));
    teamA.getCharacters().back()->setPosition({10, 3});
    teamA.getCharacters().back()->setStatus(playable);

    teamA.addCharacter(std::make_unique<Character>("Top1", "Human", 6, 3, 3, 8));
    teamA.getCharacters().back()->setPosition({10, 12});
    teamA.getCharacters().back()->setStatus(playable);

    teamA.addCharacter(std::make_unique<Character>("Top2", "Human", 6, 3, 3, 8));
    teamA.getCharacters().back()->setPosition({10, 13});
    teamA.getCharacters().back()->setStatus(playable);

    for (int i = 0; i < 5; i++) {
        if (i != 2) {
            teamA.addCharacter(std::make_unique<Character>("Behind", "Human", 6, 3, 3, 8));
            teamA.getCharacters().back()->setPosition({5, 5+i});
            teamA.getCharacters().back()->setStatus(playable);
        }

    }


    teamB.addCharacter(std::make_unique<Character>("Ligne1", "orc", 6, 3, 3, 8));
    teamB.getCharacters().back()->setPosition({13, 6});
    teamB.getCharacters().back()->setStatus(playable);

    teamB.addCharacter(std::make_unique<Character>("Ligne2", "orc", 6, 3, 3, 8));
    teamB.getCharacters().back()->setPosition({13, 7});
    teamB.getCharacters().back()->setStatus(playable);

    teamB.addCharacter(std::make_unique<Character>("Ligne3", "orc", 6, 3, 3, 8));
    teamB.getCharacters().back()->setPosition({13, 8});
    teamB.getCharacters().back()->setStatus(playable);

    teamB.addCharacter(std::make_unique<Character>("Bottom1", "orc", 6, 3, 3, 8));
    teamB.getCharacters().back()->setPosition({13, 2});
    teamB.getCharacters().back()->setStatus(playable);

    teamB.addCharacter(std::make_unique<Character>("Bottom2", "orc", 6, 3, 3, 8));
    teamB.getCharacters().back()->setPosition({13, 3});
    teamB.getCharacters().back()->setStatus(playable);

    teamB.addCharacter(std::make_unique<Character>("Top1", "orc", 6, 3, 3, 8));
    teamB.getCharacters().back()->setPosition({15, 12});
    teamB.getCharacters().back()->setStatus(playable);

    teamB.addCharacter(std::make_unique<Character>("Top2", "orc", 6, 3, 3, 8));
    teamB.getCharacters().back()->setPosition({15, 13});
    teamB.getCharacters().back()->setStatus(playable);

    for (int i = 0; i < 5; i++) {
        if (i != 2) {
            teamB.addCharacter(std::make_unique<Character>("Behind", "orc", 6, 3, 3, 8));
            teamB.getCharacters().back()->setPosition({20, 5+i});
            teamB.getCharacters().back()->setStatus(playable);
        }
    }

    // Initialize game
    BloodBowlGame game(teamA, teamB);
    
    cout << "Initial Teams:\n";
    cout << "Team A (Humans):\n" << teamA << "\n";
    cout << "Team B (Orcs):\n" << teamB << "\n";

    cout << "Coin toss winner: Team " << game.getCurrentTeam()->getTeamId() << "\n";

    cout << "\n=== INITIAL GAME STATE ===\n";
    cout << game << endl;

    // Test state machine progression
    cout << "\n=== TESTING STATE MACHINE ===\n";
    for (int round = 1; round <= 8; round++) {
        cout << "\n--- Round " << round << " ---\n";
        AbstractState *currentState = game.getCurrentState();
        cout << "Before update:\n" << game << "\n";
        // Simulate conditions based on current state to trigger transitions
        if (currentState == game.getStateList().at(SETUP).get()) {
            cout << "SETUP state detected - checking setup validity...\n";
            Setup* setupState = static_cast<Setup*>(currentState);
            // Check if both teams have valid setups
            bool teamAValid = setupState->isValidSetup(game.getTeamA());
            bool teamBValid = setupState->isValidSetup(game.getTeamB());
            cout << "Team A setup valid: " << (teamAValid ? "YES" : "NO") << "\n";
            cout << "  - Characters on board: " << setupState->nbCharacterOnBoard(game.getTeamA()) << " (max 11)\n";
            cout << "  - Characters on line: " << setupState->nbCharacterOnLine(game.getTeamA()) << " (min 3)\n";
            cout << "  - Characters on top: " << setupState->nbCharacterOnTop(game.getTeamA()) << " (max 2)\n";
            cout << "  - Characters on bottom: " << setupState->nbCharacterOnBottom(game.getTeamA()) << " (max 2)\n";
            cout << "Team B setup valid: " << (teamBValid ? "YES" : "NO") << "\n";
            cout << "  - Characters on board: " << setupState->nbCharacterOnBoard(game.getTeamB()) << " (max 11)\n";
            cout << "  - Characters on line: " << setupState->nbCharacterOnLine(game.getTeamB()) << " (min 3)\n";
            cout << "  - Characters on top: " << setupState->nbCharacterOnTop(game.getTeamB()) << " (max 2)\n";
            cout << "  - Characters on bottom: " << setupState->nbCharacterOnBottom(game.getTeamB()) << " (max 2)\n";
            if (teamAValid && teamBValid) {
                cout << "Both setups valid - ending setup to trigger transition...\n";
                setupState->endSetup();
            } else {
                cout << "Invalid setup - staying in SETUP state\n";
            }
        }
        else if (currentState == game.getStateList().at(KICKOFF).get()) {
            cout << "KICKOFF state detected - will automatically transition to PLAYERTURN\n";
        }
        else if (currentState == game.getStateList().at(PLAYERTURN).get()) {
            cout << "PLAYERTURN state detected - simulating player actions...\n";
            PlayerTurn* playerTurnState = static_cast<PlayerTurn*>(currentState);
            // Simulate different outcomes based on round
            if (round <= 3) {
                cout << "Simulating normal turn end for team " << game.getCurrentTeam()->getTeamId() << "\n";
                playerTurnState->setEndTurn(true);
            } else if (round == 4) {
                cout << "Simulating TOUCHDOWN for team " << game.getCurrentTeam()->getTeamId() << "!\n";
                playerTurnState->setTouchDown(true);
            } else {
                cout << "Simulating TurnOver for team " << game.getCurrentTeam()->getTeamId() << "\n";
                playerTurnState->setTurnOver(true);
            }
        }
        else if (currentState == game.getStateList().at(HALFTIME).get()) {
            cout << "HALFTIME state detected - will transition back to SETUP\n";
        }
        else if (currentState == game.getStateList().at(ENDGAME).get()) {
            cout << "ENDGAME state detected - game over\n";
        }
        if (currentState) {
            currentState->update();
        }
        cout << "After update:\n" << game << "\n";
        // Stop if we reach endgame
        if (game.getCurrentState() == game.getStateList().at(ENDGAME).get()) {
            cout << "Game has ended!\n";
            break;
        }
    }

    // Test team switching and scoring
    cout << "\n=== TESTING SCORING ===\n";
    cout << "Before scoring:\n" << game << "\n";
    
    game.getCurrentTeam()->setScore(game.getCurrentTeam()->getScore() + 1);
    cout << "After touchdown:\n" << game << "\n";

    // Test character status changes
    cout << "\n=== TESTING CHARACTER STATUS CHANGES ===\n";
    auto& characters = teamA.getCharacters();
    if (!characters.empty()) {
        cout << "Character example: " << *characters[0] << "\n";
        // This shows the improved status display you implemented
    }

    cout << "\n=== FINAL STATE ===\n";
    cout << game << "\n";
    
    cout << "\nTest completed successfully!\n";
    return 0;
}
