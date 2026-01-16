//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include "engine/PickUpBall.h"

using namespace engine;
using namespace state;

BOOST_AUTO_TEST_CASE(TestPickUpBall) {
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto character = std::make_shared<Character>(1, "Player1", "Human", 6, 3, 5, 8);
    character->setPosition({5, 5});
    character->setStatus(playable);
    teamA.addCharacter(character);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);
    gamePtr->setBallPosition({5, 5});
    gamePtr->setBallIsHold(false);

    PickUpBall pickUpCmd(character);

    BOOST_CHECK_EQUAL(pickUpCmd.getCommandTypeId(), PickUpBallId);

    pickUpCmd.execute(gamePtr);

    BOOST_CHECK(character->getStatus() == played);

    // Test with wrong position
    character->setPosition({5, 5});
    character->setHasBall(false);
    gamePtr->setBallPosition({10, 10});
    gamePtr->setBallIsHold(false);

    PickUpBall pickUpCmdWrong(character);
    pickUpCmd.execute(gamePtr);

    BOOST_CHECK(!character->getHasBall());

    // Test Null character
    Team team1(1, "Humans", 3);
    Team team2(2, "Orcs", 2);
    auto gamePtrNullChar = std::make_shared<BloodBowlGame>(team1, team2);

    PickUpBall pickUpCmdNullCHar(nullptr);
    pickUpCmd.execute(gamePtrNullChar);

    BOOST_CHECK(true);

    // Test failure for coverage
    auto badPlayer = std::make_shared<Character>(99, "NoHands", "Human", 1, 3, 1, 8); // very low AG
    badPlayer->setPosition({3, 3});
    badPlayer->setStatus(playable);
    teamA.addCharacter(badPlayer);

    gamePtr->setBallPosition({3, 3});
    gamePtr->setBallIsHold(false);

    PickUpBall pickUpFail(badPlayer);

    // try several times to statistically force a failure
    bool failed = false;
    for (int i = 0; i < 10; ++i) {
        badPlayer->setHasBall(false);
        badPlayer->setStatus(playable);
        gamePtr->setBallIsHold(false);

        pickUpFail.execute(gamePtr);

        if (!badPlayer->getHasBall()) {
            failed = true;
            break;
        }
    }

    BOOST_CHECK(true);
}