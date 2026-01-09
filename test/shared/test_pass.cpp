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

    auto passer = std::make_shared<Character>(1, "Passer", "Human", 6, 3, 4, 8);
    passer->setPosition({5, 5});
    passer->setHasBall(false); // No ball
    teamA.addCharacter(passer);

    auto receiver = std::make_shared<Character>(2, "Receiver", "Human", 6, 3, 4, 8);
    receiver->setPosition({8, 5});
    teamA.addCharacter(receiver);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);

    // Test constructor
    Pass passCmd(passer, receiver);

    BOOST_CHECK_EQUAL(passCmd.getCommandTypeId(), PassId);

    // Test without ball
    passCmd.execute(gamePtr);

    BOOST_CHECK(!receiver->getHasBall());

    // Test interception
    auto interceptor = std::make_shared<Character>(3, "Interceptor", "Orc", 6, 3, 4, 9);
    interceptor->setPosition({7, 5}); // On the pass line
    interceptor->setStatus(playable);
    teamB.addCharacter(interceptor);

    auto interceptors = passCmd.checkInterceptions(gamePtr);

    BOOST_CHECK(!interceptors.empty());
}