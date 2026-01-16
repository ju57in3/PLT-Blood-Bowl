//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include <state/PlayerTurn.h>

#include "engine/Move.h"

using namespace engine;
using namespace state;

BOOST_AUTO_TEST_CASE(TestMove) {
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto character = std::make_shared<Character>(1, "Player1", "Human", 6, 3, 100, 8);
    character->setPosition({5, 5});
    character->setStatus(playable);
    teamA.addCharacter(character);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);

    std::pair<int, int> targetPos = {7, 6};
    Move moveCmd(character, targetPos);

    BOOST_CHECK_EQUAL(moveCmd.getCommandTypeId(), MoveId);

    moveCmd.execute(gamePtr);

    BOOST_CHECK(character->getPosition() == targetPos);
    BOOST_CHECK(character->getStatus() == played);

    // Test with ball
    character->setPosition({5, 5});
    gamePtr->setBallPosition({5, 5});
    gamePtr->setBallIsHold(true);
    character->setHasBall(true);
    targetPos = {6, 6};
    Move moveCmdBall(character, targetPos);

    moveCmdBall.execute(gamePtr);

    BOOST_CHECK(character->getPosition() == targetPos);
    BOOST_CHECK(gamePtr->getBallPosition() == targetPos);
    BOOST_CHECK(character->getHasBall());

    BOOST_CHECK(not moveCmd.checkTackleZones());

    // Test pickup bal
    character->setPosition({5, 5});
    gamePtr->setBallPosition({6, 6});
    gamePtr->setBallIsHold(false);
    character->setHasBall(false);
    gamePtr->setCurrentState(gamePtr->getStateList().at(PLAYERTURN).get());
    auto* pt = dynamic_cast<state::PlayerTurn*>(gamePtr->getCurrentState());
    pt->setTurnOver(true);

    Move moveCmdPickup(character, {6, 6});
    moveCmdPickup.execute(gamePtr);

    // Should attempt to pick up ball (success depends on agility roll)
    BOOST_CHECK(character->getPosition() == std::make_pair(6, 6));

    // Test pickup for team B
    auto characterB = std::make_shared<Character>(2, "Player2", "Orc", 6, 3, 100, 8);
    characterB->setPosition({20, 5});
    characterB->setStatus(playable);
    teamB.addCharacter(characterB);

    // Place ball at target position for Team B character
    std::pair<int, int> targetPosB = {20, 6};
    gamePtr->setBallPosition(targetPosB);
    gamePtr->setBallIsHold(false);
    characterB->setHasBall(false);

    gamePtr->setCurrentState(gamePtr->getStateList().at(PLAYERTURN).get());
    auto* ptB = dynamic_cast<state::PlayerTurn*>(gamePtr->getCurrentState());
    ptB->setTurnOver(true);

    Move moveCmdB(characterB, targetPosB);

    BOOST_CHECK_EQUAL(moveCmdB.getCommandTypeId(), MoveId);

    moveCmdB.execute(gamePtr);

    BOOST_CHECK(characterB->getPosition() == targetPosB);
}