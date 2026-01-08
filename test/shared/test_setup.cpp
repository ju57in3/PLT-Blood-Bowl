//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestSetup)
{
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    for (int i = 0; i < 11; i++) {
        auto c = std::make_unique<Character>(i+1,"H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c->setStatus(playable);
        if (i < 3) c->setPosition({12, 4+i});      // line
        else if (i < 5) c->setPosition({10, 4+i}); // top
        else c->setPosition({8, 2+i});             // middle
        teamA.addCharacter(std::move(c));
    }

    // Initializing game with two different teams
    BloodBowlGame game(teamA, teamB);
    Setup* setup = dynamic_cast<Setup*>(game.getStateList()[SETUP].get());
    BOOST_REQUIRE(setup != nullptr);

    BOOST_CHECK(setup->isValidSetup(teamA));
    BOOST_CHECK_EQUAL(setup->nbCharacterOnBoard(teamA), 11);
    BOOST_CHECK_EQUAL(setup->nbCharacterOnLine(teamA), 3);
    BOOST_CHECK_EQUAL(setup->nbCharacterOnTop(teamA), 2);
    BOOST_CHECK_EQUAL(setup->nbCharacterOnBottom(teamA), 0);

    setup->endSetup();
    // After one team finishes, setup is not ended yet (need both teams)
    BOOST_CHECK(!setup->getSetupEnded());

    // Now finish setup for the second team
    setup->endSetup();
    BOOST_CHECK(setup->getSetupEnded());
}