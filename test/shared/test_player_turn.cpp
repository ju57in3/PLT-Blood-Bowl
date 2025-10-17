//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestPlayerTurn)
{
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    Character h1("H1", "Human", 6, 3, 3, 8);
    h1.setStatus(playable);
    h1.setPosition({5,5});
    teamA.addCharacter(h1);

    Character o1("O1", "Orc", 5, 4, 2, 9);
    o1.setStatus(playable);
    o1.setPosition({10,5});
    teamB.addCharacter(o1);

    BloodBowlGame game(teamA, teamB);

    PlayerTurn playerTurn(&game);

    BOOST_CHECK_EQUAL(playerTurn.getTurnOver(), false);
    BOOST_CHECK_EQUAL(playerTurn.getTouchDown(), false);

    // Simulates a normal ride
    playerTurn.setTurnOver(true);
    BOOST_CHECK(playerTurn.getTurnOver());

    // Simulate a touchdown
    playerTurn.setTouchDown(true);
    BOOST_CHECK(playerTurn.getTouchDown());

    // Check that the score increases
    int oldScore = game.getCurrentTeam()->getScore();
    playerTurn.update();
    BOOST_CHECK(game.getCurrentTeam()->getScore() >= oldScore);
}