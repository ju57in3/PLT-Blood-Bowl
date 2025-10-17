
#include <boost/test/unit_test.hpp>

#include "../shared/state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestTeam) {
    Team teamA(1, "Humans", 3);
    BOOST_CHECK_EQUAL(teamA.teamId,1);
    BOOST_CHECK_EQUAL(teamA.getScore(),0);
    //Tester rerolls
    //Tester name

    teamA.setScore(1);
    BOOST_CHECK_EQUAL(teamA.getScore(),1);

    Character human1("Player 1", "human", 6, 3, 3, 8);
    human1.setPosition({1,1});
    teamA.addCharacter(human1);
    Character human2("Player 2", "human", 6, 3, 3, 8);
    human2.setStatus(playable);
    human2.setPosition({2,2});
    teamA.addCharacter(human2);
    int team_size = size(teamA.getCharacters());
    BOOST_CHECK_EQUAL(team_size, 2);
    int playable = size(teamA.getPlayableCharacter());
    BOOST_CHECK_EQUAL(playable,1);

    std::ostringstream oss;
    oss << teamA;
    std::string expected =  "Team ID: 1\n"
                            "Score: 1\n"
                            "Characters:\n"
                            "Position: (1, 1) | Status: bench\n"
                            "Position: (2, 2) | Status: playable\n";

    BOOST_CHECK_EQUAL(oss.str(), expected);
}

/* vim: set sw=2 sts=2 et : */
