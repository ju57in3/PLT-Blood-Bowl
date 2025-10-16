
#include <boost/test/unit_test.hpp>

#include "../shared/state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestCharacter)
{
    Character human("Player 1", "human", 6, 3, 3, 8);
    BOOST_CHECK_EQUAL(human.getStatus(),bench);
    human.setStatus(playable);
    BOOST_CHECK_EQUAL(human.getStatus(),playable);

    BOOST_CHECK(human.getPosition() == std::make_pair(-1,-1));
    human.setPosition(std::make_pair(1,2));
    BOOST_CHECK(human.getPosition() == std::make_pair(1,2));

    BOOST_CHECK(not human.hasBall());
}

/* vim: set sw=2 sts=2 et : */