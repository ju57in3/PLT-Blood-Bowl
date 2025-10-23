//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestHalfTime)
{
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);
    BloodBowlGame game(teamA, teamB);

    HalfTime halfTime(&game);

    // Simulation of the KO check (depending on implementation)
    halfTime.checkKO(&game);

    // Update = transition to SETUP
    halfTime.update();
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[SETUP].get());
}