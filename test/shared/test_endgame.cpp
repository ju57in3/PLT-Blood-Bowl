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
    auto cA1 = std::make_shared<Character>(1,"H1", "Human", 6, 3, 3, 8);
    auto cA2 = std::make_shared<Character>(2,"H2", "Human", 6, 3, 3, 8);
    auto cB1 = std::make_shared<Character>(1,"O1", "Orc", 5, 3, 3, 9);

    cA1->setStatus(playable);
    cA2->setStatus(playable);
    cB1->setStatus(playable);

    teamA.addCharacter(cA1);
    teamA.addCharacter(cA2);
    teamB.addCharacter(cB1);

    // Creation of the game and state EndGame
    BloodBowlGame game(teamA, teamB);
    EndGame endgame(&game);

    // Execution of update()
    endgame.update();

    BOOST_CHECK(not endgame.getShouldRestart());
    endgame.setShouldRestart(true);
    BOOST_CHECK(endgame.getShouldRestart());

    BOOST_CHECK(not endgame.getShouldQuit());
    endgame.setShouldQuit(true);
    BOOST_CHECK(endgame.getShouldQuit());

    endgame.update();

    std::string expectedStr = "EndGame";
    BOOST_CHECK(endgame.getName() == expectedStr);
}