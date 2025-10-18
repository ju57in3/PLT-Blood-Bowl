//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestKickoff)
{
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);
    BloodBowlGame game(teamA, teamB);

    // Creation of Kickoff state
    Kickoff kickoff(&game);

    // We execute the kick-off action
    kickoff.update();

    // Check that the position is inside the field
    BOOST_CHECK_GE(game.getBallPosition().first, 0);                  // x >= 0
    BOOST_CHECK_GE(game.getBallPosition().second, 0);                 // y >= 0
    BOOST_CHECK_LT(game.getBallPosition().first, game.getWidth());    // x < width
    BOOST_CHECK_LT(game.getBallPosition().second, game.getHeight());  // y < height

    // Checks that the current state is PlayerTurn
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[PLAYERTURN].get());
}