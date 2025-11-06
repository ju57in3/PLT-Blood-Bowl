//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestEndGame)
{
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    // Dynamic allocation cause EndGame loop on Characers
    auto cA1 = std::make_shared<Character>("H1", "Human", 6, 3, 3, 8);
    auto cA2 = std::make_shared<Character>("H2", "Human", 6, 3, 3, 8);
    auto cB1 = std::make_shared<Character>("O1", "Orc", 5, 3, 3, 9);

    cA1->setStatus(playable);
    cA2->setStatus(playable);
    cB1->setStatus(playable);

    teamA.addCharacter(cA1);
    teamA.addCharacter(cA2);
    teamB.addCharacter(cB1);

    // Creation of the game and state EndGame
    BloodBowlGame game(teamA, teamB);
    EndGame endgame(&game);

    // Verify initial state
    BOOST_CHECK_EQUAL(endgame.getRestart(), false);

    // Activate restart to start the loop
    endgame.setRestart(true);
    BOOST_CHECK_EQUAL(endgame.getRestart(), true);

    // Execution of update()
    endgame.update();

    // Verify if all characters are on bench
    for (auto& c : game.getTeamA().getCharacters()) {
        BOOST_CHECK(c != nullptr);
        BOOST_CHECK_EQUAL(c->getStatus(), bench);
    }
    for (auto& c : game.getTeamB().getCharacters()) {
        BOOST_CHECK(c != nullptr);
        BOOST_CHECK_EQUAL(c->getStatus(), bench);
    }

    endgame.update();

    endgame.setRestart(false);
    BOOST_CHECK_EQUAL(endgame.getRestart(), false);
}