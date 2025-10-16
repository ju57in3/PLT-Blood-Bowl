#include <iostream>
#include <string>

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

    for (int i = 0; i < 11; i++) {
        Character human("Player" + to_string(i+1), "Human", 6, 3, 3, 8);
        if (i < 3) {
            human.setPosition({12, 4+i}); 
            human.setStatus(playable);
        } else if (i < 5) {
            human.setPosition({11, 4+i});
            human.setStatus(playable);
        } else if (i < 9) {
            human.setPosition({8, 3+i});
            human.setStatus(playable);
        } else {
            human.setPosition({-1, -1});
            human.setStatus(bench);
        }
        teamA.addCharacter(human);
    }

    for (int i = 0; i < 11; i++) {
        Character orc("Orc" + to_string(i+1), "Orc", 5, 3, 2, 9);
        if (i < 3) {
            orc.setPosition({13, 2+i});
            orc.setStatus(playable);
        } else if (i < 9) {
            orc.setPosition({15, 3+i});
            orc.setStatus(playable);
        } else {
            orc.setPosition({-1, -1});
            orc.setStatus(bench);
        }
        teamB.addCharacter(orc);
    }

    // Initialize game
    BloodBowlGame game(teamA, teamB);
    
    cout << "Initial Teams:\n";
    cout << "Team A (Humans):\n" << teamA << "\n";
    cout << "Team B (Orcs):\n" << teamB << "\n";

    cout << "Coin toss winner: Team " << game.getCurrentTeam().getTeamId() << "\n";

    cout << "\n=== INITIAL GAME STATE ===\n";
    cout << game << endl;

    // Test state machine progression
    cout << "\n=== TESTING STATE MACHINE ===\n";
    
    for (int round = 1; round <= 8; round++) {
        cout << "\n--- Round " << round << " ---\n";
        
        AbstractState* currentState = game.getCurrentState();
        
        cout << "Before update:\n" << game << "\n";
        
        // Simulate conditions based on current state to trigger transitions
        if (currentState == game.getStateList()[SETUP]) {
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
        else if (currentState == game.getStateList()[KICKOFF]) {
            cout << "KICKOFF state detected - will automatically transition to PLAYERTURN\n";
        }
        else if (currentState == game.getStateList()[PLAYERTURN]) {
            cout << "PLAYERTURN state detected - simulating player actions...\n";
            
            PlayerTurn* playerTurnState = static_cast<PlayerTurn*>(currentState);
            
            // Simulate different outcomes based on round
            if (round <= 3) {
                cout << "Simulating normal turn end for team " << game.getCurrentTeam().getTeamId() << "\n";
                playerTurnState->simulateEndTurn();
            } else if (round == 4) {
                cout << "Simulating TOUCHDOWN for team " << game.getCurrentTeam().getTeamId() << "!\n";
                playerTurnState->simulateTouchdown();
            } else {
                cout << "Simulating turn end for team " << game.getCurrentTeam().getTeamId() << "\n";
                playerTurnState->simulateEndTurn();
            }
        }
        else if (currentState == game.getStateList()[HALFTIME]) {
            cout << "HALFTIME state detected - will transition back to SETUP\n";
        }
        else if (currentState == game.getStateList()[ENDGAME]) {
            cout << "ENDGAME state detected - game over\n";
        }
        
        if (currentState) {
            currentState->update();
        }
        
        cout << "After update:\n" << game << "\n";
        
        // Stop if we reach endgame
        if (game.getCurrentState() == game.getStateList()[ENDGAME]) {
            cout << "Game has ended!\n";
            break;
        }
    }

    // Test team switching and scoring
    cout << "\n=== TESTING SCORING ===\n";
    cout << "Before scoring:\n" << game << "\n";
    
    game.getCurrentTeam().setScore(game.getCurrentTeam().getScore() + 1);
    cout << "After touchdown:\n" << game << "\n";

    // Test character status changes
    cout << "\n=== TESTING CHARACTER STATUS CHANGES ===\n";
    auto characters = teamA.getCharacters();
    if (characters.size() > 0) {
        cout << "Character example: " << characters[0] << "\n";
        // This shows the improved status display you implemented
    }

    cout << "\n=== FINAL STATE ===\n";
    cout << game << "\n";
    
    cout << "\nTest completed successfully!\n";
    return 0;
}
