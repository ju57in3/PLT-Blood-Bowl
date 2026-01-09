//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
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
    targetPos = {6, 6};
    Move moveCmdBall(character, targetPos);

    moveCmdBall.execute(gamePtr);

    BOOST_CHECK(character->getPosition() == targetPos);
    BOOST_CHECK(gamePtr->getBallPosition() == targetPos);
    BOOST_CHECK(character->getHasBall());

    // Test out of range
    character->setPosition({5, 5});
    targetPos = {20, 20};
    Move moveCmdOutofRange(character, targetPos);

    auto initialPos = character->getPosition();
    moveCmdOutofRange.execute(gamePtr);

    BOOST_CHECK(character->getPosition() == initialPos);

    // Test pickup bal
    character->setPosition({5, 5});
    gamePtr->setBallPosition({6, 6});
    gamePtr->setBallIsHold(false);

    Move moveCmdPickup(character, {6, 6});
    moveCmdPickup.execute(gamePtr);

    // Should attempt to pick up ball (success depends on agility roll)
    BOOST_CHECK(character->getPosition() == std::make_pair(6, 6));

    // Test
}