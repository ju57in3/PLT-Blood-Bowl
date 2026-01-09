//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"
#include <cstdlib>

using namespace state;

BOOST_AUTO_TEST_CASE(TestHalfTime)
{
    // Creation of teams with KO characters
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    // Dynamics characters
    auto cA1 = std::make_shared<Character>(1,"H1", "Human", 6, 3, 3, 8);
    auto cB1 = std::make_shared<Character>(2,"O1", "Orc", 5, 3, 2, 9);
    auto cB2 = std::make_shared<Character>(3,"O2", "Orc", 5, 3, 2, 9);

    // We put characters KO
    cA1->setStatus(ko);
    cB1->setStatus(ko);
    cB2->setStatus(playable);

    teamA.addCharacter(cA1);
    teamB.addCharacter(cB1);
    teamB.addCharacter(cB2);

    // Creation of game and HalfTime state
    BloodBowlGame game(teamA, teamB);
    HalfTime* halfTime = dynamic_cast<HalfTime*>(game.getStateList()[HALFTIME].get());
    BOOST_REQUIRE(halfTime != nullptr);

    // Seed random for reproducibility
    srand(42);

    // Simulation of the KO check (depending on implementation)
    halfTime->checkKO(&game);

    // Verify that KO characters either stayed KO or went to bench (random outcome)
    for (auto& c : game.getTeamA().getCharacters()) {
        BOOST_CHECK(c != nullptr);
        // Character was KO, so should be either still KO or on bench now
        BOOST_CHECK(c->getStatus() == ko || c->getStatus() == bench);
    }

    // At least verify that the KO check doesn't crash and characters are valid
    for (auto& c : game.getTeamB().getCharacters()) {
        BOOST_CHECK(c != nullptr);
        // The playable character should still be playable
        if (c->getId() == 3) {
            BOOST_CHECK_EQUAL(c->getStatus(), playable);
        } else {
            // KO characters should be either KO or bench
            BOOST_CHECK(c->getStatus() == ko || c->getStatus() == bench);
        }
    }

    // Update = transition to SETUP
    halfTime->update();
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[SETUP].get());

    std::string expectedStr = "HalfTime";
    BOOST_CHECK(halfTime->getName() == expectedStr);
}