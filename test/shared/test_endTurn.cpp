//
// Created by matt-o on 1/16/26.
//
#include <boost/test/unit_test.hpp>
#include "engine/EndTurn.h"
#include "state/PlayerTurn.h"

using namespace engine;
using namespace state;

BOOST_AUTO_TEST_CASE(TestEndTurn) {
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);
    EndTurn endTurn;

    BOOST_CHECK_EQUAL(endTurn.getCommandTypeId(), EndTurnId);

    gamePtr->setCurrentState(gamePtr->getStateList().at(PLAYERTURN).get());
    endTurn.execute(gamePtr);
    BOOST_CHECK_EQUAL(gamePtr->getCurrentTeam(), &teamB);

    // Test with unexpected setup
    endTurn.execute(nullptr);
    BOOST_CHECK_EQUAL(gamePtr->getCurrentTeam(), &teamB);

    gamePtr->setCurrentState(gamePtr->getStateList().at(KICKOFF).get());
    endTurn.execute(gamePtr);
    BOOST_CHECK_EQUAL(gamePtr->getCurrentTeam(), &teamB);

}