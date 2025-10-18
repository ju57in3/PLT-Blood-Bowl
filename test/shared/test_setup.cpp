//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestSetup)
{
    Team teamA(1, "Humans", 3);
    for (int i = 0; i < 11; i++) {
        Character c("H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c.setStatus(playable);
        if (i < 3) c.setPosition({13, 4+i});      // Line
        else if (i < 5) c.setPosition({12, 2+i}); // Top
        else c.setPosition({8, 4+i});             // Middle
        teamA.addCharacter(c);
    }

    BloodBowlGame game(teamA, teamA);
    Setup setup(&game);

    BOOST_CHECK(setup.isValidSetup(teamA));
    BOOST_CHECK_EQUAL(setup.nbCharacterOnBoard(teamA), 11);
    BOOST_CHECK_EQUAL(setup.nbCharacterOnLine(teamA), 2);
    BOOST_CHECK_EQUAL(setup.nbCharacterOnTop(teamA), 3);
    BOOST_CHECK_EQUAL(setup.nbCharacterOnBottom(teamA), 0);

    setup.endSetup();
    BOOST_CHECK(setup.getSetupEnded());
}