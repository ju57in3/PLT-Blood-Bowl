
#include <boost/test/unit_test.hpp>

#include "../shared/state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestTeam) {
    Team teamA(1, "Humans", 3);
    BOOST_CHECK_EQUAL(teamA.teamId,1);
    BOOST_CHECK_EQUAL(teamA.getScore(),0);
    BOOST_CHECK_EQUAL(teamA.getRerolls(), 3);
    BOOST_CHECK_EQUAL(teamA.getName(),"Humans");

    teamA.setScore(1);
    BOOST_CHECK_EQUAL(teamA.getScore(),1);

    teamA.setRerolls(2);
    BOOST_CHECK_EQUAL(teamA.getRerolls(),2);

    teamA.setName("Humans Team");
    BOOST_CHECK_EQUAL(teamA.getName(),"Humans Team");

    BOOST_CHECK_EQUAL(size(teamA.getCharacters()), 0);
    Character human("Player 1", "human", 6, 3, 3, 8);
    human.setPosition({1,1});
    teamA.addCharacter(human);
    BOOST_CHECK_EQUAL(size(teamA.getCharacters()), 1);
    BOOST_CHECK_EQUAL(teamA.getCharacters()[0].getName(),"Player 1");
    BOOST_CHECK_EQUAL(teamA.getCharacters()[0].getType(),"human");
    BOOST_CHECK_EQUAL(teamA.getCharacters()[0].getMovement(),6);
    BOOST_CHECK_EQUAL(teamA.getCharacters()[0].getStrength(),3);
    BOOST_CHECK_EQUAL(teamA.getCharacters()[0].getAgility(),3);
    BOOST_CHECK_EQUAL(teamA.getCharacters()[0].getArmor(),8);

    BOOST_CHECK_EQUAL(size(teamA.getPlayableCharacter()),0);
    teamA.getCharacters()[0].setStatus(playable);
    human.setStatus(playable);
    BOOST_CHECK_EQUAL(size(teamA.getPlayableCharacter()),1);

    std::ostringstream oss;
    oss << teamA;
    std::string expected =  "Team ID: 1\n"
                            "Score: 1\n"
                            "Characters:\n"
                            "Position: (1, 1) | Status: playable\n";

    BOOST_CHECK_EQUAL(oss.str(), expected);
}

/* vim: set sw=2 sts=2 et : */
