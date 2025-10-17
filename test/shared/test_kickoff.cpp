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

    Kickoff kickoff(&game);

    // Initial ball position
    kickoff.kickBall({5, 5});
    BOOST_CHECK_EQUAL(game.getBallPosition().first, 2);
    BOOST_CHECK_EQUAL(game.getBallPosition().second, 2);

    // Call update() must transition to PLAYERTURN
    kickoff.update();
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[PLAYERTURN]);
}