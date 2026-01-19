//
// Created by matt-o on 1/15/26.
//
#include <boost/test/unit_test.hpp>

#include "engine/Block.h"
#include "utility/GameUtils.h"

using namespace engine;
using namespace state;
using namespace utility;

BOOST_AUTO_TEST_CASE(TestEngine) {
    // Set up RNG
    GameUtils::getRNG().seed(42);

    // Set up game, team and characters
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto attacker = std::make_shared<Character>(1, "Attacker", "Human", 6, 3, 6, 8);
    attacker->setPosition({5, 5});
    teamA.addCharacter(attacker);

    auto defender = std::make_shared<Character>(2, "Defender", "Orc", 6, 3, 6, 8);
    defender->setPosition({5, 6});
    teamB.addCharacter(defender);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);

    // Test constructor
    Block block(attacker, defender);
    BOOST_CHECK_EQUAL(block.getCommandTypeId(),BlockId);
    BOOST_CHECK(!block.getDiceOptions().empty());

    block.applyDiceChoice(1);

}