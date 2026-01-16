//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include <utility/Constants.h>

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
        else if (i < 7) c->setPosition({10, 2+i-7}); // bot
        else c->setPosition({8, 2+i});             // middle
        teamA.addCharacter(std::move(c));
    }

    // Initializing game with two different teams
    BloodBowlGame game(teamA, teamB);
    Setup* setup = dynamic_cast<Setup*>(game.getStateList()[SETUP].get());
    BOOST_REQUIRE(setup != nullptr);

    setup->update();
    std::array<bool,2> expected = {false, false};
    BOOST_CHECK(setup->teamSetupDone == expected);

    BOOST_CHECK(setup->isValidSetup(teamA));
    BOOST_CHECK_EQUAL(setup->nbCharacterOnBoard(teamA), 11);
    BOOST_CHECK_EQUAL(setup->nbCharacterOnLine(teamA), 3);
    BOOST_CHECK_EQUAL(setup->nbCharacterOnLine(teamB), 0);
    BOOST_CHECK_EQUAL(setup->nbCharacterOnTop(teamA), 2);
    BOOST_CHECK_EQUAL(setup->nbCharacterOnBottom(teamA), 2);

    setup->endSetup();
    // After one team finishes, setup is not ended yet (need both teams)
    BOOST_CHECK(!setup->getSetupEnded());


    // Now finish setup for the second team
    setup->endSetup();
    BOOST_CHECK(setup->getSetupEnded());

    std::string expectedStr = "Setup";
    BOOST_CHECK(setup->getName() == expectedStr);


    // Testing update
    expected = {true, true};
    BOOST_CHECK(setup->teamSetupDone == expected);
    setup->update();
    expected = {false, false};
    BOOST_CHECK(setup->teamSetupDone == expected);

    // Testing wrong setup and
    for (int i = 0; i < 12; i++) { // too much characters
        auto c = std::make_unique<Character>(i+1,"O" + std::to_string(i+1), "Orc", 6, 3, 3, 8);
        c->setStatus(playable);
        if (i < 2) c->setPosition({13, 4+i});      // Only 2 on line
        else if (i < 5) c->setPosition({16, utility::Constants::BOARD_HEIGHT-4+i-1}); // too much on top
        else if (i < 8) c->setPosition({14, i-5}); // too luch on bot
        else c->setPosition({15+i, 6});             // middle
        teamB.addCharacter(std::move(c));
    }
    BOOST_CHECK(!setup->isValidSetup(teamB));
    auto chars = teamB.getCharacters();
    chars[11]->setStatus(bench);
    BOOST_CHECK(!setup->isValidSetup(teamB));
    chars[10]->setPosition({13, 7});
    BOOST_CHECK(!setup->isValidSetup(teamB));
    chars[3]->setPosition({16, 5});
    BOOST_CHECK(!setup->isValidSetup(teamB));
    chars[6]->setPosition({14, 8});
    BOOST_CHECK(setup->isValidSetup(teamB));

}