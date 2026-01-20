//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include <state/PlayerTurn.h>

#include "engine/Engine.h"
#include "utility/Constants.h"

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
    hum->setPosition({20, 5});
    hum->setStatus(playable);
    teamA.addCharacter(hum);

    auto orc = std::make_shared<Character>(1, "Orc1", "Orc", 6, 3, 3, 9);
    orc->setPosition({1, 5});
    orc->setStatus(playable);
    teamB.addCharacter(orc);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);

    // Test base Command methods
    DummyCommand baseCmd;
    BOOST_CHECK_EQUAL(baseCmd.getCommandTypeId(), 0);

    // Test execute does nothing (base implementation)
    baseCmd.execute(gamePtr);
    BOOST_CHECK(true);

    // Test touchdown both teams
    gamePtr->setCurrentState(gamePtr->getStateList().at(PLAYERTURN).get());
    gamePtr->setBallPosition({24, 5});
    baseCmd.checkAndHandleTouchdown(gamePtr);
    BOOST_CHECK_EQUAL(teamA.getScore(), 0);

    hum->setHasBall(true);
    gamePtr->setBallIsHold(true);
    std::pair<int, int> targetPos = {utility::Constants::BOARD_WIDTH - 1, 5};
    gamePtr->setBallPosition(targetPos);

    baseCmd.checkAndHandleTouchdown(gamePtr);
    BOOST_CHECK_EQUAL(teamA.getScore(), 1);

    gamePtr->setCurrentTeam(&gamePtr->getTeamB());
    hum->setHasBall(false);
    targetPos = {0, 5};
    gamePtr->setBallPosition(targetPos);
    orc->setHasBall(true);

    baseCmd.checkAndHandleTouchdown(gamePtr);
    BOOST_CHECK_EQUAL(teamB.getScore(), 1);

    // Test Turnover
    baseCmd.checkAndHandleTurnover(gamePtr);
    auto* pt = dynamic_cast<state::PlayerTurn*>(gamePtr->getCurrentState());
    BOOST_CHECK(pt->getTurnOver());
}