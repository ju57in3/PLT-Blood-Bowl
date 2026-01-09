//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include "engine/Pass.h"

using namespace engine;
using namespace state;

class DummyCommand : public Command {
public:
    explicit DummyCommand() : Command() {}
};

BOOST_AUTO_TEST_CASE(TestCommand) {
    // Seting up
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto hum = std::make_shared<Character>(1, "Player1", "Human", 6, 3, 3, 8);
    hum->setPosition({24, 5}); // Near TD zone team A
    hum->setStatus(playable);
    teamA.addCharacter(hum);

    auto orc = std::make_shared<Character>(1, "Orc1", "Orc", 6, 3, 3, 9);
    orc->setPosition({1, 5});
    orc->setStatus(playable);
    orc->setHasBall(true);
    teamB.addCharacter(orc);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);

    // Test base Command methods
    DummyCommand baseCmd;
    BOOST_CHECK_EQUAL(baseCmd.getCommandTypeId(), 0);

    // Test execute does nothing (base implementation)
    baseCmd.execute(gamePtr);
    BOOST_CHECK(true);

}