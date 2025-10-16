
#include <boost/test/unit_test.hpp>

#include "../shared/state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestCharacter)
{
    Character human("Player 1", "human", 6, 3, 3, 8);
    human.getStatus();
}

/* vim: set sw=2 sts=2 et : */
