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
    BloodBowlGame game(teamA, teamB);

    EndGame endgame(&game);

    BOOST_CHECK_EQUAL(endgame.restart, false);

    endgame.update();

    // End of game test (no transition)
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[ENDGAME]);
}