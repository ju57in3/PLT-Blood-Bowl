
#include <boost/test/unit_test.hpp>

#include "../shared/state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestCharacter)
{
    Character human("Player 1", "human", 6, 3, 3, 8);
    BOOST_CHECK_EQUAL(human.getStatus(),bench);
    BOOST_CHECK(human.getPosition() == std::make_pair(-1,-1));
    BOOST_CHECK(not human.getHasBall());
    BOOST_CHECK_EQUAL(human.getMovement(),6);
    BOOST_CHECK_EQUAL(human.getStrength(),3);
    BOOST_CHECK_EQUAL(human.getAgility(),3);
    BOOST_CHECK_EQUAL(human.getArmor(),8);
    BOOST_CHECK_EQUAL(human.getType(),"human");
    BOOST_CHECK_EQUAL(human.getName(),"Player 1");

    human.setStatus(playable);
    BOOST_CHECK_EQUAL(human.getStatus(),playable);
    human.setName("Player One");
    BOOST_CHECK_EQUAL(human.getName(),"Player One");

    human.setMovement(7);
    BOOST_CHECK_EQUAL(human.getMovement(),7);
    human.setStrength(4);
    BOOST_CHECK_EQUAL(human.getStrength(),4);
    human.setAgility(4);
    BOOST_CHECK_EQUAL(human.getAgility(),4);
    human.setArmor(9);
    BOOST_CHECK_EQUAL(human.getArmor(),9);

    human.setType("orc");
    BOOST_CHECK_EQUAL(human.getName(),"orc");

    human.setHasBall(true);
    BOOST_CHECK(human.getHasBall());

    human.setPosition(std::make_pair(1,2));
    BOOST_CHECK(human.getPosition() == std::make_pair(1,2));

    std::ostringstream oss;
    oss << human;
    std::string expected =  "Position: (1, 2) | Status: playable";
    BOOST_CHECK_EQUAL(oss.str(), expected);
}

/* vim: set sw=2 sts=2 et : */