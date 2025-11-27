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

    auto cA = std::make_unique<Character>(1,"H1", "Human", 6, 3, 3, 8);
    cA->setStatus(playable);
    cA->setPosition({5,5});
    teamA.addCharacter(std::move(cA));

    auto cB = std::make_unique<Character>(2,"O1", "Orcs", 5, 4, 2, 9);
    cB->setStatus(playable);
    cB->setPosition({10,5});
    teamB.addCharacter(std::move(cB));
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

    // Coverage : update() with no score, no turn end
    PlayerTurn noChange(&game);
    noChange.setTurnOver(false);
    noChange.setTouchDown(false);
    noChange.update();
    BOOST_CHECK_EQUAL(noChange.getTurnOver(), false);

    // Coverage : restoreCharactersStatus
    auto cPlayed = std::make_unique<Character>(1,"H2", "Human", 5, 4, 4, 6);
    cPlayed->setStatus(played);
    teamA.addCharacter(std::move(cPlayed));

    auto cStunned = std::make_unique<Character>(2,"H3", "Human", 5, 4, 4, 6);
    cStunned->setStatus(stunned);
    teamA.addCharacter(std::move(cStunned));

    game.setCurrentTeam(&teamA);
    playerTurn.restoreCharactersStatus();
    for (auto& c : teamA.getCharacters()) {
        if (c->getName() == "H2") BOOST_CHECK_EQUAL(c->getStatus(), playable);
        if (c->getName() == "H3") BOOST_CHECK_EQUAL(c->getStatus(), knockedDown);
    }

    // Coverage: switchTeam() else branch
    game.setCurrentTeam(&teamB);
    playerTurn.switchTeam();
    BOOST_CHECK_EQUAL(game.getCurrentTeam()->getTeamId(), teamA.getTeamId());

    // Coverage: tc == 16
    game.setCurrentTeam(&teamA);
    game.setTurnCounter(15);
    playerTurn.setTurnOver(true);
    playerTurn.update();
    BOOST_CHECK(game.getTurnCounter() == 16);

    // Coverage: tc == 32
    game.setTurnCounter(31);
    playerTurn.setTurnOver(true);
    playerTurn.update();
    BOOST_CHECK(game.getTurnCounter() == 32);

    // Coverage: else branch, return to PLAYERTURN
    game.setTurnCounter(5);
    playerTurn.setTurnOver(true);
    playerTurn.setTouchDown(false);
    playerTurn.setEndTurn(false);
    playerTurn.update();

    BOOST_CHECK(game.getCurrentState() == game.getStateList().at(PLAYERTURN).get());

    // Coverage: get/setEndTurn()
    playerTurn.setEndTurn(true);
    BOOST_CHECK(playerTurn.getEndTurn());
}
