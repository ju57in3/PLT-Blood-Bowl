
#include <boost/test/unit_test.hpp>

#include "../shared/state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestCharacter)
{
    Character human("Player 1", "human", 6, 3, 3, 8);
    BOOST_CHECK_EQUAL(human.getStatus(),bench);
    BOOST_CHECK(human.getPosition() == std::make_pair(-1,-1));
    BOOST_CHECK(not human.hasBall());
    BOOST_CHECK_EQUAL(getMovement(),6);
    BOOST_CHECK_EQUAL(getStrength(),3);
    BOOST_CHECK_EQUAL(getAgility(),3);
    BOOST_CHECK_EQUAL(getArmor(),8);
    BOOST_CHECK_EQUAL(getType(),"human");
    BOOST_CHECK_EQUAL(getName(),"Player 1");

    human.setStatus(playable);
    BOOST_CHECK_EQUAL(human.getStatus(),playable);

    human.setPosition(std::make_pair(1,2));
    BOOST_CHECK(human.getPosition() == std::make_pair(1,2));

    std::ostringstream oss;
    oss << human;
    std::string expected =  "Position: (1, 2) | Status: playable";
    BOOST_CHECK_EQUAL(oss.str(), expected);
}

/* vim: set sw=2 sts=2 et : */