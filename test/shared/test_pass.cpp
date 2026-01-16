//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include "engine/Pass.h"

using namespace engine;
using namespace state;

BOOST_AUTO_TEST_CASE(TestPass) {
    // Set up game, team and characters
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto passer = std::make_shared<Character>(1, "Passer", "Human", 6, 3, 100, 8); // High agility to better chance to success
    passer->setPosition({5, 5});
    passer->setHasBall(false); // No ball
    teamA.addCharacter(passer);

    auto receiver = std::make_shared<Character>(2, "Receiver", "Human", 6, 3, 100, 8);
    receiver->setPosition({8, 5});
    teamA.addCharacter(receiver);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);

    // Test constructor without ball
    Pass passNoBall(passer, receiver);

    BOOST_CHECK_EQUAL(passNoBall.getCommandTypeId(), PassId);

    // Test without ball
    passNoBall.execute(gamePtr);

    BOOST_CHECK(!receiver->getHasBall());

    // Test Successful Quick range pass (distance <= 3)
    passer->setHasBall(true);
    passer->setStatus(playable);
    receiver->setPosition({7, 5}); // Distance = 2
    gamePtr->setBallIsHold(true);
    gamePtr->setBallPosition({5, 5});
    auto initBallPos = passer->getPosition();

    Pass passQuick(passer, receiver);
    passQuick.execute(gamePtr);

    BOOST_CHECK(initBallPos != gamePtr->getBallPosition()); // We just verify that the ball moved has the succes depends on agility
    BOOST_CHECK(!passer->getHasBall());
    BOOST_CHECK(passer->getStatus() == played);

    // Test Successful Short range pass (distance 4-6)
    passer->setHasBall(true);
    passer->setStatus(playable);
    receiver->setPosition({10, 5}); // Distance = 5
    gamePtr->setBallIsHold(true);
    gamePtr->setBallPosition({5, 5});

    Pass passShort(passer, receiver);
    passShort.execute(gamePtr);

    BOOST_CHECK(initBallPos != gamePtr->getBallPosition());
    BOOST_CHECK(!passer->getHasBall());
    BOOST_CHECK(passer->getStatus() == played);

    // Test Long range pass (distance 7-9)
    passer->setHasBall(true);
    passer->setStatus(playable);
    receiver->setHasBall(false);
    receiver->setPosition({13, 5}); // Distance = 8
    gamePtr->setBallIsHold(true);

    Pass passLong(passer, receiver);
    passLong.execute(gamePtr);

    BOOST_CHECK(initBallPos != gamePtr->getBallPosition());
    BOOST_CHECK(!passer->getHasBall());
    BOOST_CHECK(passer->getStatus() == played);

    // Test Long Bomb range (distance 10-12)
    passer->setHasBall(true);
    passer->setStatus(playable);
    receiver->setHasBall(false);
    receiver->setPosition({16, 5}); // Distance = 11
    gamePtr->setBallIsHold(true);

    Pass passLongBomb(passer, receiver);
    passLongBomb.execute(gamePtr);
    BOOST_CHECK(initBallPos != gamePtr->getBallPosition());
    BOOST_CHECK(!passer->getHasBall());
    BOOST_CHECK(passer->getStatus() == played);

    // Test Impossible range (distance > 12)
    passer->setHasBall(true);
    passer->setStatus(playable);
    receiver->setHasBall(false);
    receiver->setPosition({20, 5}); // Distance = 15
    gamePtr->setBallIsHold(true);

    Pass passImpossible(passer, receiver);
    passImpossible.execute(gamePtr);

    BOOST_CHECK(!passer->getHasBall());
    BOOST_CHECK(!gamePtr->getBallIsHold());
    BOOST_CHECK(passer->getStatus() == played);

    // Test interception
    passer->setHasBall(true);
    passer->setStatus(playable);
    receiver->setHasBall(false);
    receiver->setPosition({16, 5});
    gamePtr->setBallIsHold(true);
    gamePtr->setBallPosition({5, 5});
    auto interceptor = std::make_shared<Character>(3, "Interceptor", "Orc", 6, 3, 5, 8); // High agility (5) to increase success chance
    interceptor->setPosition({7, 5}); // On the pass line
    interceptor->setStatus(playable);
    teamB.addCharacter(interceptor);

    auto interceptors = passNoBall.checkInterceptions(gamePtr);

    BOOST_CHECK(!interceptors.empty());

    // 10 Times with high agility to increase success chance for interception
    for (int i = 0; i < 10; i++) {
        passer->setHasBall(true);
        passer->setStatus(playable);
        receiver->setHasBall(false);
        gamePtr->setBallIsHold(true);
        gamePtr->setBallPosition({5, 5});

        Pass passInterceptors(passer, receiver);
        passInterceptors.execute(gamePtr);
    }

    BOOST_CHECK(initBallPos != gamePtr->getBallPosition());
    BOOST_CHECK(!passer->getHasBall());

    // Test for team B condition
    auto passerB = std::make_shared<Character>(1, "Passer", "Orc", 6, 3, 4, 8);
    passerB->setPosition({20, 5});
    passerB->setHasBall(false); // No ball
    teamB.addCharacter(passerB);

    auto receiverB = std::make_shared<Character>(2, "Receiver", "Orc", 6, 3, 4, 8);
    receiverB->setPosition({15, 5});
    teamB.addCharacter(receiverB);
    Pass passCmdB(passerB, receiverB);
    interceptors.clear();
    interceptors = passCmdB.checkInterceptions(gamePtr);

    BOOST_CHECK(interceptors.empty());

    passerB->setHasBall(true);
    passerB->setStatus(playable);
    receiverB->setPosition({20, 3});
    gamePtr->setBallIsHold(true);
    gamePtr->setBallPosition({20, 5});

    Pass passTeamB(passerB, receiverB);
    passTeamB.execute(gamePtr);

    BOOST_CHECK(initBallPos != gamePtr->getBallPosition());
    BOOST_CHECK(!passerB->getHasBall());
    BOOST_CHECK(passerB->getStatus() == played);

}