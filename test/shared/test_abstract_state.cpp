//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestAbstractState)
{
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);
    BloodBowlGame game(teamA, teamB);

    // Checks that a concrete instance is accessible via AbstractState*
    AbstractState* state = game.getCurrentState();
    BOOST_CHECK(state != nullptr);

    // Polymorphic virtual call
    state->update();

    // Checks that the state remains consistent
    BOOST_CHECK(game.getCurrentState() != nullptr);
}