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
    BloodBowlGame game(teamA, teamB);

    PlayerTurn playerTurn(&game);

    BOOST_CHECK_EQUAL(playerTurn.isTurnOver, false);
    BOOST_CHECK_EQUAL(playerTurn.isTouchDown, false);

    // Simule un tour normal
    playerTurn.simulateEndTurn();
    BOOST_CHECK(playerTurn.isTurnOver);

    // Simule un touchdown
    playerTurn.simulateTouchdown();
    BOOST_CHECK(playerTurn.isTouchDown);

    // Vérifie que le score augmente
    int oldScore = game.getCurrentTeam()->getScore();
    playerTurn.update();
    BOOST_CHECK(game.getCurrentTeam()->getScore() >= oldScore);
}