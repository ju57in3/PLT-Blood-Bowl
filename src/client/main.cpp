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

    Character human1("Ligne1", "Human", 6, 3, 3, 8);
    human1.setPosition({12, 6});
    human1.setStatus(playable);
    teamA.addCharacter(human1);
    Character human2("Ligne2", "Human", 6, 3, 3, 8);
    human2.setPosition({12, 7});
    human2.setStatus(playable);
    teamA.addCharacter(human2);
    Character human3("Ligne3", "Human", 6, 3, 3, 8);
    human3.setPosition({12, 8});
    human3.setStatus(playable);
    teamA.addCharacter(human3);

    Character human4("Bottom1", "Human", 6, 3, 3, 8);
    human4.setPosition({10, 2});
    human4.setStatus(playable);
    teamA.addCharacter(human4);
    Character human5("Bottom2", "Human", 6, 3, 3, 8);
    human5.setPosition({10, 3});
    human5.setStatus(playable);
    teamA.addCharacter(human5);

    Character human6("Top1", "Human", 6, 3, 3, 8);
    human6.setPosition({10, 12});
    human6.setStatus(playable);
    teamA.addCharacter(human6);
    Character human7("Top2", "Human", 6, 3, 3, 8);
    human7.setPosition({10, 13});
    human7.setStatus(playable);
    teamA.addCharacter(human7);

    for (int i = 0; i < 5; i++) {
        if (i != 2) {
            Character human("Behind", "Human", 6, 3, 3, 8);
            human.setPosition({5, 5+i});
            human.setStatus(playable);
            teamA.addCharacter(human);
        }

    }


    Character orc1("Ligne1", "orc", 6, 3, 3, 8);
    orc1.setPosition({13, 6});
    orc1.setStatus(playable);
    teamB.addCharacter(orc1);
    Character orc2("Ligne2", "orc", 6, 3, 3, 8);
    orc2.setPosition({13, 7});
    orc2.setStatus(playable);
    teamB.addCharacter(orc2);
    Character orc3("Ligne3", "orc", 6, 3, 3, 8);
    orc3.setPosition({13, 8});
    orc3.setStatus(playable);
    teamB.addCharacter(orc3);

    Character orc4("Bottom1", "orc", 6, 3, 3, 8);
    orc4.setPosition({13, 2});
    orc4.setStatus(playable);
    teamB.addCharacter(orc4);
    Character orc5("Bottom2", "orc", 6, 3, 3, 8);
    orc5.setPosition({13, 3});
    orc5.setStatus(playable);
    teamB.addCharacter(orc5);

    Character orc6("Top1", "orc", 6, 3, 3, 8);
    orc6.setPosition({15, 12});
    orc6.setStatus(playable);
    teamB.addCharacter(orc6);
    Character orc7("Top2", "orc", 6, 3, 3, 8);
    orc7.setPosition({15, 13});
    orc7.setStatus(playable);
    teamB.addCharacter(orc7);

    for (int i = 0; i < 5; i++) {
        if (i != 2) {
            Character orc("Behind", "orc", 6, 3, 3, 8);
            orc.setPosition({20, 5+i});
            orc.setStatus(playable);
            teamB.addCharacter(orc);
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
        cout << "Character example: " << characters[0] << "\n";
        // This shows the improved status display you implemented
    }

    cout << "\n=== FINAL STATE ===\n";
    cout << game << "\n";
    
    cout << "\nTest completed successfully!\n";
    return 0;
}
