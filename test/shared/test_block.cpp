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
    //Set up RNG
    GameUtils::getRNG().seed(42);

    //Set up game, teams and characters
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto attacker = std::make_shared<Character>(1, "Attacker", "Human", 6, 3, 3, 8);
    attacker->setPosition({5, 5});
    teamA.addCharacter(attacker);

    auto defender = std::make_shared<Character>(2, "Defender", "Human", 6, 3, 3, 5);
    defender->setPosition({8, 5});
    teamA.addCharacter(defender);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);

    // Test number of dices
    Block block1Dice(attacker, defender);
    block1Dice.generateDiceOptions();
    std::vector<int> diceOptions = block1Dice.getDiceOptions();
    BOOST_CHECK_EQUAL(diceOptions.size(), 1); //We give the attacker 3 strength points and the defender 3

    attacker->setStrength(4); // > defender->getStrength()
    Block block2Dice(attacker, defender);
    diceOptions = block2Dice.getDiceOptions();
    BOOST_CHECK_EQUAL(diceOptions.size(), 2);

    attacker->setStrength(6); // >= 2 * defender->getStrength()
    Block block3Dice(attacker, defender);
    diceOptions = block3Dice.getDiceOptions();
    BOOST_CHECK_EQUAL(diceOptions.size(), 3);


    // Choice of the dice result
    block3Dice.applyDiceChoice(1);

}