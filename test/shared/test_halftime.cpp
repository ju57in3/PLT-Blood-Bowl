//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

// Mock of rand() to force diceRoll = 1
int fake_rand_value = 1;
int fake_rand(){ return fake_rand_value; }
#define rand() fake_rand()

using namespace state;

BOOST_AUTO_TEST_CASE(TestHalfTime)
{
    // Creation of teams with KO characters
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    // Dynamics characters
    auto cA1 = std::make_shared<Character>("H1", "Human", 6, 3, 3, 8);
    auto cB1 = std::make_shared<Character>("O1", "Orc", 5, 3, 2, 9);
    auto cB2 = std::make_shared<Character>("O2", "Orc", 5, 3, 2, 9);

    // We put a character KO
    cA1->setStatus(ko);
    cB1->setStatus(ko);
    cB2->setStatus(playable);

    teamA.addCharacter(cA1);
    teamB.addCharacter(cB1);
    teamB.addCharacter(cB2);

    // Creation of game and HalfTime state
    BloodBowlGame game(teamA, teamB);
    HalfTime halfTime(&game);

    // Simulation of the KO check (depending on implementation)
    halfTime.checkKO(&game);

    // Verify that the KO characters are on the bench
    for (auto& c : game.getTeamA().getCharacters()) {
        BOOST_CHECK(c != nullptr);
        BOOST_CHECK_EQUAL(c->getStatus(), bench);
    }

    bool orcKOfound = false;
    for (auto& c : game.getTeamB().getCharacters()) {
        BOOST_CHECK(c != nullptr);
        if (c->getStatus() == bench) {
            orcKOfound = true;
        }
    }
    BOOST_CHECK(orcKOfound);

    // Update = transition to SETUP
    halfTime.update();
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[SETUP].get());
}