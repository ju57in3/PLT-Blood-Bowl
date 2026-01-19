
#include <boost/test/unit_test.hpp>

#include "../shared/state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestTeam) {
    Team teamA(1, "Humans", 3);
    BOOST_CHECK_EQUAL(teamA.teamId,1);
    BOOST_CHECK_EQUAL(teamA.getScore(),0);
    BOOST_CHECK_EQUAL(teamA.getRerolls(), 3);
    BOOST_CHECK_EQUAL(teamA.getName(),"Humans");

    teamA.setRerolls(2);
    BOOST_CHECK_EQUAL(teamA.getRerolls(),2);

    teamA.setName("Humans Team");
    BOOST_CHECK_EQUAL(teamA.getName(),"Humans Team");

    BOOST_CHECK_EQUAL(size(teamA.getCharacters()), 0);
    auto human = std::make_shared<Character>(1,"Player 1", "human", 6, 3, 3, 8);
    human->setPosition({1,1});
    teamA.addCharacter(human);
    BOOST_CHECK_EQUAL(size(teamA.getCharacters()), 1);
    BOOST_CHECK_EQUAL(human->getName(),"Player 1");
    BOOST_CHECK_EQUAL(human->getType(),"human");
    BOOST_CHECK_EQUAL(human->getMovement(),6);
    BOOST_CHECK_EQUAL(human->getStrength(),3);
    BOOST_CHECK_EQUAL(human->getAgility(),3);
    BOOST_CHECK_EQUAL(human->getArmor(),8);

    BOOST_CHECK_EQUAL(size(teamA.getPlayableCharacter()),0);
    human->setStatus(playable);
    BOOST_CHECK_EQUAL(size(teamA.getPlayableCharacter()),1);

    teamA.setScore(1);
    BOOST_CHECK_EQUAL(teamA.getScore(),1);
    teamA.resetMatchData();
    BOOST_CHECK_EQUAL(teamA.getScore(),0);

    std::ostringstream oss;
    oss << teamA;
    std::string expected =  "Team ID: 1\n"
                            "Score: 0\n"
                            "Characters:\n"
                            "Position: (1, 1) | Status: playable\n";

    BOOST_CHECK_EQUAL(oss.str(), expected);

    for (int i = 0; i < 15; i++) {
        auto c = std::make_unique<Character>(i+1,"H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c->setStatus(playable);
        teamA.addCharacter(std::move(c));
    }
    auto extra = std::make_shared<Character>(16,"Extra","Human", 6, 3, 3, 8);
    teamA.addCharacter(extra);
    BOOST_CHECK_EQUAL(teamA.getCharacters().size(), 16);

}

/* vim: set sw=2 sts=2 et : */
