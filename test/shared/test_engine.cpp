//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include "engine/Engine.h"
#include "engine/Move.h"

using namespace engine;
using namespace state;

// Mock AI class for testing
class MockAI : public ai::AI {
public:
    MockAI(Engine& engine, const std::shared_ptr<BloodBowlGame>& game, int teamId)
        : AI(engine, game, teamId) {
        runAICalled = false;
        placePlayersCalled = false;
    }

    bool runAI() override {
        runAICalled = true;
        return true;
    }

    void placePlayers() {
        placePlayersCalled = true;

        // Use the parent class implementation
        AI::placePlayers();
    }

    bool runAICalled;
    bool placePlayersCalled;
};

BOOST_AUTO_TEST_CASE(TestEngine) {
    // Set up et test for basic methods
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);
    Engine engine(gamePtr);

    BOOST_CHECK(engine.getGameState() != nullptr);

    auto gamePtr2 = std::make_shared<BloodBowlGame>(teamA, teamB);
    engine.setGameState(gamePtr2);
    BOOST_CHECK(engine.getGameState() == gamePtr2);

    // Test for execute
    auto character = std::make_shared<Character>(1, "Player1", "Human", 6, 3, 3, 8);
    character->setPosition({5, 5});
    character->setStatus(playable);
    teamA.addCharacter(character);

    std::pair<int, int> targetPos = {6, 6};
    auto moveCmd = std::make_unique<Move>(character, targetPos);

    engine.addCommand(std::move(moveCmd));
    engine.executeCommand();

    BOOST_CHECK(character->getPosition() == targetPos);

    // Test for fallbacks
    engine.executeCommand();
    BOOST_CHECK(true);

    Engine engineNullGame(nullptr);
    auto character2 = std::make_shared<Character>(2, "Player2", "Human", 6, 3, 3, 8);
    character2->setPosition({5, 5});
    auto moveCmd2 = std::make_unique<Move>(character2, std::make_pair(7, 7));
    engineNullGame.addCommand(std::move(moveCmd2));
    engineNullGame.executeCommand();
    BOOST_CHECK(true);

    engine.addCommand(std::move(nullptr));
    BOOST_CHECK(true);

    // ========================================================================
    // Test for AI
    // ========================================================================
    // setAI and getAI
    Team teamA_AI(1, "Humans", 3);
    Team teamB_AI(2, "Orcs", 2);

    auto gamePtr3 = std::make_shared<BloodBowlGame>(teamA_AI, teamB_AI);
    Engine engineAI(gamePtr3);

    auto mockAI = std::make_unique<MockAI>(engineAI, gamePtr3, 1);
    auto* mockAIPtr = mockAI.get();

    engineAI.setAI(std::move(mockAI));
    BOOST_CHECK(engineAI.getAI() != nullptr);
    BOOST_CHECK(engineAI.getAI() == mockAIPtr);

    // Test fallback AI
    engineAI.setAI(nullptr);
    BOOST_CHECK(engineAI.getAI() == nullptr);

    Engine engineNoAI(gamePtr3);
    engineNoAI.runAITurnIfNeeded();
    BOOST_CHECK(true);

    Engine engineNullGameAI(nullptr);
    auto mockAI2 = std::make_unique<MockAI>(engineNullGameAI, nullptr, 1);
    engineNullGameAI.setAI(std::move(mockAI2));
    engineNullGameAI.runAITurnIfNeeded();
    BOOST_CHECK(true);

    // runAITurnIfNeeded when it's AI's turn
    Team teamA2(1, "Humans", 3);
    Team teamB2(2, "Orcs", 2);

    for (int i = 0; i < 11; i++) {
        auto c = std::make_unique<Character>(i+1, "H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c->setStatus(playable);
        c->setPosition({5 + i, 5});
        teamA2.addCharacter(std::move(c));
    }

    auto gamePtr4 = std::make_shared<BloodBowlGame>(teamA2, teamB2);
    Engine engineAITurn(gamePtr4);

    auto mockAI3 = std::make_unique<MockAI>(engineAITurn, gamePtr4, 1); // AI controls team 1
    auto* mockAI3Ptr = mockAI3.get();
    engineAITurn.setAI(std::move(mockAI3));

    // Simulate that it's team 1's turn
    engineAITurn.runAITurnIfNeeded();

    BOOST_CHECK(engineAITurn.getAI() != nullptr);

    // runAITurnIfNeeded when it's NOT AI's turn
    auto mockAI4 = std::make_unique<MockAI>(engineAITurn, gamePtr4, 2); // AI controls team 2

    Engine engineAINotTurn(gamePtr4);
    engineAINotTurn.setAI(std::move(mockAI4));

    // If current team is 1, AI (team 2) should not run
    engineAINotTurn.runAITurnIfNeeded();
    BOOST_CHECK(engineAINotTurn.getAI() != nullptr);

    // runAITurnIfNeeded prevents double execution
    auto mockAI5 = std::make_unique<MockAI>(engineAITurn, gamePtr4, 1);
    auto* mockAI5Ptr = mockAI5.get();

    Engine engineAIDouble(gamePtr4);
    engineAIDouble.setAI(std::move(mockAI5));

    // Call twice - AI should only run once per team turn
    engineAIDouble.runAITurnIfNeeded();
    bool firstCall = mockAI5Ptr->runAICalled;
    mockAI5Ptr->runAICalled = false;

    engineAIDouble.runAITurnIfNeeded();
    bool secondCall = mockAI5Ptr->runAICalled;

    // Second call should not trigger AI if it's still the same team's turn
    BOOST_CHECK(!secondCall);

    // Test reset flag when other team plays
    Team teamA_reset(1, "Humans", 3);
    Team teamB_reset(2, "Orcs", 2);
    auto gamePtr_reset = std::make_shared<BloodBowlGame>(teamA_reset, teamB_reset);

    Engine engineReset(gamePtr_reset);
    auto mockAI_reset = std::make_unique<MockAI>(engineReset, gamePtr_reset, 1);
    auto* mockAI_resetPtr = mockAI_reset.get();
    engineReset.setAI(std::move(mockAI_reset));

    // Team 1 plays
    gamePtr_reset->setCurrentTeam(&teamA_reset);
    engineReset.runAITurnIfNeeded();
    BOOST_CHECK(mockAI_resetPtr->runAICalled);
    mockAI_resetPtr->runAICalled = false;

    // Team 2 plays - resets flag
    gamePtr_reset->setCurrentTeam(&teamB_reset);
    engineReset.runAITurnIfNeeded();
    BOOST_CHECK(!mockAI_resetPtr->runAICalled); // Team 2, AI doesn't play

    // Team 1 plays again - AI should play again
    gamePtr_reset->setCurrentTeam(&teamA_reset);
    engineReset.runAITurnIfNeeded();
    BOOST_CHECK(mockAI_resetPtr->runAICalled);

    // runAITurnIfNeeded with no current team
    auto gamePtr_noTeam = std::make_shared<BloodBowlGame>(teamA, teamB);
    Engine engineNoCurrentTeam(gamePtr_noTeam);
    auto mockAI_noTeam = std::make_unique<MockAI>(engineNoCurrentTeam, gamePtr_noTeam, 1);
    engineNoCurrentTeam.setAI(std::move(mockAI_noTeam));
    gamePtr_noTeam->setCurrentTeam(nullptr);
    engineNoCurrentTeam.runAITurnIfNeeded();
    BOOST_CHECK(true);

    // ========================================================================
    // Test runAISetupIfNeeded
    // ========================================================================

    // runAISetupIfNeeded with nullptr setup
    Engine engineNullSetup(gamePtr4);
    auto mockAI6 = std::make_unique<MockAI>(engineNullSetup, gamePtr4, 1);
    engineNullSetup.setAI(std::move(mockAI6));
    engineNullSetup.runAISetupIfNeeded(nullptr);
    BOOST_CHECK(true);

    // runAISetupIfNeeded with no AI
    Team teamA_noAI(1, "Humans", 3);
    Team teamB_noAI(2, "Orcs", 2);
    auto gamePtr_noAI = std::make_shared<BloodBowlGame>(teamA_noAI, teamB_noAI);
    Setup* setup_noAI = dynamic_cast<Setup*>(gamePtr_noAI->getStateList()[SETUP].get());

    Engine engineSetupNoAI(gamePtr_noAI);
    engineSetupNoAI.runAISetupIfNeeded(setup_noAI);
    BOOST_CHECK(true);

    // runAISetupIfNeeded with null game
    Engine engineSetupNullGame(nullptr);
    auto mockAI7 = std::make_unique<MockAI>(engineSetupNullGame, nullptr, 1);
    engineSetupNullGame.setAI(std::move(mockAI7));
    Setup setup_null(nullptr);
    engineSetupNullGame.runAISetupIfNeeded(&setup_null);
    BOOST_CHECK(true);

    // runAISetupIfNeeded with no current team
    Team teamA_noCurrentTeam(1, "Humans", 3);
    Team teamB_noCurrentTeam(2, "Orcs", 2);
    auto gamePtr_noCurrentTeamSetup = std::make_shared<BloodBowlGame>(teamA_noCurrentTeam, teamB_noCurrentTeam);
    Setup* setup_noCurrentTeam = dynamic_cast<Setup*>(gamePtr_noCurrentTeamSetup->getStateList()[SETUP].get());

    Engine engineSetupNoCurrentTeam(gamePtr_noCurrentTeamSetup);
    auto mockAI_noCurrentTeamSetup = std::make_unique<MockAI>(engineSetupNoCurrentTeam, gamePtr_noCurrentTeamSetup, 1);
    engineSetupNoCurrentTeam.setAI(std::move(mockAI_noCurrentTeamSetup));
    gamePtr_noCurrentTeamSetup->setCurrentTeam(nullptr);
    engineSetupNoCurrentTeam.runAISetupIfNeeded(setup_noCurrentTeam);
    BOOST_CHECK(true);

    // runAISetupIfNeeded when not AI's team
    Team teamA_wrongTeam(1, "Humans", 3);
    Team teamB_wrongTeam(2, "Orcs", 2);
    auto gamePtr_wrongTeam = std::make_shared<BloodBowlGame>(teamA_wrongTeam, teamB_wrongTeam);
    Setup* setup_wrongTeam = dynamic_cast<Setup*>(gamePtr_wrongTeam->getStateList()[SETUP].get());

    Engine engineSetupWrongTeam(gamePtr_wrongTeam);
    auto mockAI_wrongTeam = std::make_unique<MockAI>(engineSetupWrongTeam, gamePtr_wrongTeam, 2); // AI is team 2
    auto* mockAI_wrongTeamPtr = mockAI_wrongTeam.get();
    engineSetupWrongTeam.setAI(std::move(mockAI_wrongTeam));

    gamePtr_wrongTeam->setCurrentTeam(&teamA_wrongTeam); // Current team is 1
    engineSetupWrongTeam.runAISetupIfNeeded(setup_wrongTeam);
    BOOST_CHECK(!mockAI_wrongTeamPtr->placePlayersCalled);

    // runAISetupIfNeeded when setup already done
    Team teamA_done(1, "Humans", 3);
    Team teamB_done(2, "Orcs", 2);

    for (int i = 0; i < 3; i++) {
        auto c = std::make_unique<Character>(i+1, "H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c->setStatus(playable);
        c->setPosition({12, 6 + i});
        teamA_done.addCharacter(std::move(c));
    }

    auto gamePtr_done = std::make_shared<BloodBowlGame>(teamA_done, teamB_done);
    Setup* setup_done = dynamic_cast<Setup*>(gamePtr_done->getStateList()[SETUP].get());
    gamePtr_done->setCurrentTeam(&teamA_done);

    // Mark setup as done for team A
    setup_done->endSetup();

    Engine engineSetupDone(gamePtr_done);
    auto mockAI_done = std::make_unique<MockAI>(engineSetupDone, gamePtr_done, 1);
    auto* mockAI_donePtr = mockAI_done.get();
    engineSetupDone.setAI(std::move(mockAI_done));

    engineSetupDone.runAISetupIfNeeded(setup_done);
    BOOST_CHECK(!mockAI_donePtr->placePlayersCalled);

    // runAISetupIfNeeded successful setup
    Team teamA3(1, "Humans", 3);
    Team teamB3(2, "Orcs", 2);

    // Add 11 characters for valid setup
    for (int i = 0; i < 11; i++) {
        auto c = std::make_unique<Character>(i+1, "H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c->setStatus(playable);
        teamA3.addCharacter(std::move(c));
    }

    auto gamePtr5 = std::make_shared<BloodBowlGame>(teamA3, teamB3);
    Setup* setup = dynamic_cast<Setup*>(gamePtr5->getStateList()[SETUP].get());
    gamePtr5->setCurrentState(setup);
    gamePtr5->setCurrentTeam(&teamA3);

    Setup* setupState2 = dynamic_cast<Setup*>(gamePtr5->getStateList()[SETUP].get());
    BOOST_REQUIRE(setupState2 != nullptr);

    Engine engineSetupValid(gamePtr5);
    auto mockAI8 = std::make_unique<MockAI>(engineSetupValid, gamePtr5, 1);
    auto* mockAI8Ptr = mockAI8.get();
    engineSetupValid.setAI(std::move(mockAI8));

    // Run AI setup
    engineSetupValid.runAISetupIfNeeded(setupState2);
    BOOST_CHECK(mockAI8Ptr->placePlayersCalled);

    // Verify setup is valid and was ended
    BOOST_CHECK(setupState2->isValidSetup(teamA3));

}