
#include <boost/test/unit_test.hpp>

#include "../shared/state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestTeam)
{
    Team teamA(1, "Humans", 3);
    BOOST_CHECK_EQUAL(teamA.teamId,1);
    BOOST_CHECK_EQUAL(teamA.getScore(),0);
    teamA.setScore(1);
    BOOST_CHECK_EQUAL(teamA.getScore(),1);
    Character human("Player 1", "human", 6, 3, 3, 8);
    teamA.addCharacter(human);
}

/* vim: set sw=2 sts=2 et : */
