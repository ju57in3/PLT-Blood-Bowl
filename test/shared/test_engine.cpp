//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include "engine/Engine.h"
#include "engine/Move.h"
#include "state/Kickoff.h"

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
        AI::placePlayers();
        placePlayersCalled = true;
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

    // Test fallback set and get AI
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

    engineAITurn.runAITurnIfNeeded();

    BOOST_CHECK(engineAITurn.getAI() != nullptr);

    // runAITurnIfNeeded when it's NOT AI's turn
    auto mockAI4 = std::make_unique<MockAI>(engineAITurn, gamePtr4, 2); // AI controls team 2

    Engine engineAINotTurn(gamePtr4);
    engineAINotTurn.setAI(std::move(mockAI4));

    engineAINotTurn.runAITurnIfNeeded();
    BOOST_CHECK(engineAINotTurn.getAI() != nullptr);

    auto mockAI5 = std::make_unique<MockAI>(engineAITurn, gamePtr4, 1);
    auto* mockAI5Ptr = mockAI5.get();

    Engine engineAIDouble(gamePtr4);
    engineAIDouble.setAI(std::move(mockAI5));

    engineAIDouble.runAITurnIfNeeded();
    bool firstCall = mockAI5Ptr->runAICalled;
    mockAI5Ptr->runAICalled = false;

    engineAIDouble.runAITurnIfNeeded();
    bool secondCall = mockAI5Ptr->runAICalled;

    BOOST_CHECK(!secondCall);

    // Test reset flag when other team plays
    Team teamA_reset(1, "Humans", 3);
    Team teamB_reset(2, "Orcs", 2);
    auto gamePtr_reset = std::make_shared<BloodBowlGame>(teamA_reset, teamB_reset);

    Engine engineReset(gamePtr_reset);
    auto mockAI_reset = std::make_unique<MockAI>(engineReset, gamePtr_reset, 1);
    auto* mockAI_resetPtr = mockAI_reset.get();
    engineReset.setAI(std::move(mockAI_reset));

    gamePtr_reset->setCurrentTeam(&teamA_reset);
    engineReset.runAITurnIfNeeded();
    BOOST_CHECK(mockAI_resetPtr->runAICalled);
    mockAI_resetPtr->runAICalled = false;

    // Team 2 plays, resets flag
    gamePtr_reset->setCurrentTeam(&teamB_reset);
    engineReset.runAITurnIfNeeded();
    BOOST_CHECK(!mockAI_resetPtr->runAICalled);

    // Team 1 plays again, AI should play again
    gamePtr_reset->setCurrentTeam(&teamA_reset);
    engineReset.runAITurnIfNeeded();
    BOOST_CHECK(mockAI_resetPtr->runAICalled);

    // runAITurnIfNeeded callback
    auto gamePtr_noTeam = std::make_shared<BloodBowlGame>(teamA, teamB);
    Engine engineNoCurrentTeam(gamePtr_noTeam);
    auto mockAI_noTeam = std::make_unique<MockAI>(engineNoCurrentTeam, gamePtr_noTeam, 1);
    engineNoCurrentTeam.setAI(std::move(mockAI_noTeam));
    gamePtr_noTeam->setCurrentTeam(nullptr);
    engineNoCurrentTeam.runAITurnIfNeeded();
    BOOST_CHECK(true);

    // Test for Second AI functions
    Team teamA_secondAI(1, "Humans", 3);
    Team teamB_secondAI(2, "Orcs", 2);

    auto gamePtr_secondAI = std::make_shared<BloodBowlGame>(teamA_secondAI, teamB_secondAI);
    Engine engineSecondAI(gamePtr_secondAI);

    auto mockSecondAI = std::make_unique<MockAI>(engineSecondAI, gamePtr_secondAI, 2);
    auto* mockSecondAIPtr = mockSecondAI.get();
    engineSecondAI.setSecondAI(std::move(mockSecondAI));
    BOOST_CHECK(engineSecondAI.getSecondAI() != nullptr);
    BOOST_CHECK(engineSecondAI.getSecondAI() == mockSecondAIPtr);

    engineSecondAI.setSecondAI(nullptr);
    BOOST_CHECK(engineSecondAI.getSecondAI() == nullptr);

    // Test runAITurnIfNeeded with secondAI
    Team teamA_secondAITurn(1, "Humans", 3);
    Team teamB_secondAITurn(2, "Orcs", 2);

    auto gamePtr_secondAITurn = std::make_shared<BloodBowlGame>(teamA_secondAITurn, teamB_secondAITurn);
    Engine engineSecondAITurn(gamePtr_secondAITurn);

    auto mockSecondAI2 = std::make_unique<MockAI>(engineSecondAITurn, gamePtr_secondAITurn, 2);
    auto* mockSecondAI2Ptr = mockSecondAI2.get();
    engineSecondAITurn.setSecondAI(std::move(mockSecondAI2));

    gamePtr_secondAITurn->setCurrentTeam(&teamB_secondAITurn);
    engineSecondAITurn.runAITurnIfNeeded();
    BOOST_CHECK(mockSecondAI2Ptr->runAICalled);

    mockSecondAI2Ptr->runAICalled = false;
    engineSecondAITurn.runAITurnIfNeeded();
    BOOST_CHECK(!mockSecondAI2Ptr->runAICalled);

    // runAISetupIfNeeded fallbcaks ...
    Engine engineNullSetup(gamePtr4);
    auto mockAI6 = std::make_unique<MockAI>(engineNullSetup, gamePtr4, 1);
    engineNullSetup.setAI(std::move(mockAI6));
    engineNullSetup.runAISetupIfNeeded(nullptr);
    BOOST_CHECK(true);

    // with no AI
    Team teamA_noAI(1, "Humans", 3);
    Team teamB_noAI(2, "Orcs", 2);
    auto gamePtr_noAI = std::make_shared<BloodBowlGame>(teamA_noAI, teamB_noAI);
    Setup* setup_noAI = dynamic_cast<Setup*>(gamePtr_noAI->getStateList()[SETUP].get());

    Engine engineSetupNoAI(gamePtr_noAI);
    engineSetupNoAI.runAISetupIfNeeded(setup_noAI);
    BOOST_CHECK(true);

    // with null game
    Engine engineSetupNullGame(nullptr);
    auto mockAI7 = std::make_unique<MockAI>(engineSetupNullGame, nullptr, 1);
    engineSetupNullGame.setAI(std::move(mockAI7));
    Setup setup_null(nullptr);
    engineSetupNullGame.runAISetupIfNeeded(&setup_null);
    BOOST_CHECK(true);

    // with no current team
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

    // when not AI's team
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

    // when setup already done
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

    for (int i = 0; i < 11; i++) {
        auto c = std::make_unique<Character>(i+1, "H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c->setStatus(playable);
        teamA3.addCharacter(std::move(c));
    }

    auto gamePtr5 = std::make_shared<BloodBowlGame>(teamA3, teamB3);
    Setup* setupState2 = dynamic_cast<Setup*>(gamePtr5->getStateList()[SETUP].get());
    BOOST_REQUIRE(setupState2 != nullptr);

    gamePtr5->setCurrentState(setupState2);
    gamePtr5->setCurrentTeam(&teamA3);

    BOOST_CHECK(!setupState2->isTeamSetupDone(teamA3.getTeamId()));

    int charactersBeforeSetup = 0;
    for (const auto& c : teamA3.getCharacters()) {
        if (c && c->getStatus() == playable) {
            charactersBeforeSetup++;
        }
    }

    Engine engineSetupValid(gamePtr5);
    auto mockAI8 = std::make_unique<MockAI>(engineSetupValid, gamePtr5, 1);
    engineSetupValid.setAI(std::move(mockAI8));

    engineSetupValid.runAISetupIfNeeded(setupState2);

    // Verify setup was executed by checking characters are now positioned and playable
    int charactersAfterSetup = 0;
    for (const auto& c : teamA3.getCharacters()) {
        if (c && c->getStatus() == playable && c->getPosition() != std::make_pair(-1, -1)) {
            charactersAfterSetup++;
        }
    }

    BOOST_CHECK(charactersAfterSetup > 0);
    BOOST_CHECK(setupState2->isValidSetup(teamA3));
    BOOST_CHECK(setupState2->isTeamSetupDone(teamA3.getTeamId()));

    // Test runAISetupIfNeeded with invalid setup
    Team teamA_invalid(1, "Humans", 3);
    Team teamB_invalid(2, "Orcs", 2);

    // Add only 2 characters
    for (int i = 0; i < 2; i++) {
        auto c = std::make_unique<Character>(i+1, "H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c->setStatus(playable);
        teamA_invalid.addCharacter(std::move(c));
    }

    auto gamePtr_invalid = std::make_shared<BloodBowlGame>(teamA_invalid, teamB_invalid);
    Setup* setupState_invalid = dynamic_cast<Setup*>(gamePtr_invalid->getStateList()[SETUP].get());

    gamePtr_invalid->setCurrentState(setupState_invalid);
    gamePtr_invalid->setCurrentTeam(&teamA_invalid);

    Engine engineSetupInvalid(gamePtr_invalid);
    auto mockAI_invalid = std::make_unique<MockAI>(engineSetupInvalid, gamePtr_invalid, 1);
    engineSetupInvalid.setAI(std::move(mockAI_invalid));

    engineSetupInvalid.runAISetupIfNeeded(setupState_invalid);

    BOOST_CHECK(!setupState_invalid->isValidSetup(teamA_invalid));
    BOOST_CHECK(!setupState_invalid->isTeamSetupDone(teamA_invalid.getTeamId()));

    // Test runAISetupIfNeeded with secondAI
    Team teamA_setupSecondAI(1, "Humans", 3);
    Team teamB_setupSecondAI(2, "Orcs", 2);

    for (int i = 0; i < 11; i++) {
        auto c = std::make_unique<Character>(i+1, "H" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        c->setStatus(playable);
        c->setPosition({12, 6 + i % 7}); // Position them to complete setup
        teamA_setupSecondAI.addCharacter(std::move(c));
    }

    for (int i = 0; i < 11; i++) {
        auto c = std::make_unique<Character>(i+11, "O" + std::to_string(i+1), "Orc", 6, 3, 3, 8);
        c->setStatus(playable);
        teamB_setupSecondAI.addCharacter(std::move(c));
    }

    auto gamePtr_setupSecondAI = std::make_shared<BloodBowlGame>(teamA_setupSecondAI, teamB_setupSecondAI);
    Setup* setup_secondAI = dynamic_cast<Setup*>(gamePtr_setupSecondAI->getStateList()[SETUP].get());

    gamePtr_setupSecondAI->setCurrentState(setup_secondAI);

    gamePtr_setupSecondAI->setCurrentTeam(&teamA_setupSecondAI);
    setup_secondAI->endSetup();

    gamePtr_setupSecondAI->setCurrentTeam(&teamB_setupSecondAI);

    BOOST_CHECK(!setup_secondAI->isTeamSetupDone(teamB_setupSecondAI.getTeamId()));

    Engine engineSetupSecondAI(gamePtr_setupSecondAI);
    auto mockAI_setupSecondAI = std::make_unique<MockAI>(engineSetupSecondAI, gamePtr_setupSecondAI, 2);
    engineSetupSecondAI.setSecondAI(std::move(mockAI_setupSecondAI));

    engineSetupSecondAI.runAISetupIfNeeded(setup_secondAI);

    int positionedPlayers = 0;
    for (const auto& c : teamB_setupSecondAI.getCharacters()) {
        if (c && c->getStatus() == playable && c->getPosition() != std::make_pair(-1, -1)) {
            positionedPlayers++;
        }
    }
    BOOST_CHECK(positionedPlayers > 0);
    BOOST_CHECK(setup_secondAI->isTeamSetupDone(teamB_setupSecondAI.getTeamId()));

    // Test runAIKickoffIfNeeded team A
    Team teamA_kickoffA(1, "Humans", 3);
    Team teamB_kickoffA(2, "Orcs", 2);
    auto gamePtr_kickoffA = std::make_shared<BloodBowlGame>(teamA_kickoffA, teamB_kickoffA);
    Kickoff* kickoff_A = dynamic_cast<Kickoff*>(gamePtr_kickoffA->getStateList()[KICKOFF].get());

    gamePtr_kickoffA->setCurrentTeam(&teamA_kickoffA);

    Engine engineKickoffA(gamePtr_kickoffA);
    auto mockAI_kickoffA = std::make_unique<MockAI>(engineKickoffA, gamePtr_kickoffA, 1);
    engineKickoffA.setAI(std::move(mockAI_kickoffA));

    engineKickoffA.runAIKickoffIfNeeded(kickoff_A);
    BOOST_CHECK(kickoff_A->getTargetSelected());

    // Test runAIKickoffIfNeeded team B
    Team teamA_kickoffB(1, "Humans", 3);
    Team teamB_kickoffB(2, "Orcs", 2);
    auto gamePtr_kickoffB = std::make_shared<BloodBowlGame>(teamA_kickoffB, teamB_kickoffB);
    Kickoff* kickoff_B = dynamic_cast<Kickoff*>(gamePtr_kickoffB->getStateList()[KICKOFF].get());

    gamePtr_kickoffB->setCurrentTeam(&teamB_kickoffB);

    Engine engineKickoffB(gamePtr_kickoffB);
    auto mockAI_kickoffB = std::make_unique<MockAI>(engineKickoffB, gamePtr_kickoffB, 2);
    engineKickoffB.setAI(std::move(mockAI_kickoffB));

    engineKickoffB.runAIKickoffIfNeeded(kickoff_B);
    BOOST_CHECK(kickoff_B->getTargetSelected());

    // runAIKickoffIfNeeded fallbacks
    Engine engineNullKickoff(gamePtr4);
    auto mockAI_nullKickoff = std::make_unique<MockAI>(engineNullKickoff, gamePtr4, 1);
    engineNullKickoff.setAI(std::move(mockAI_nullKickoff));
    engineNullKickoff.runAIKickoffIfNeeded(nullptr);
    BOOST_CHECK(true);

    Engine engineKickoffNullGame(nullptr);
    auto mockAI_kickoffNullGame = std::make_unique<MockAI>(engineKickoffNullGame, nullptr, 1);
    engineKickoffNullGame.setAI(std::move(mockAI_kickoffNullGame));
    Kickoff kickoff_nullGame(nullptr);
    engineKickoffNullGame.runAIKickoffIfNeeded(&kickoff_nullGame);
    BOOST_CHECK(true);

    Team teamA_kickoffNoTeam(1, "Humans", 3);
    Team teamB_kickoffNoTeam(2, "Orcs", 2);
    auto gamePtr_kickoffNoTeam = std::make_shared<BloodBowlGame>(teamA_kickoffNoTeam, teamB_kickoffNoTeam);
    Kickoff* kickoff_noTeam = dynamic_cast<Kickoff*>(gamePtr_kickoffNoTeam->getStateList()[KICKOFF].get());

    Engine engineKickoffNoTeam(gamePtr_kickoffNoTeam);
    auto mockAI_kickoffNoTeam = std::make_unique<MockAI>(engineKickoffNoTeam, gamePtr_kickoffNoTeam, 1);
    engineKickoffNoTeam.setAI(std::move(mockAI_kickoffNoTeam));
    gamePtr_kickoffNoTeam->setCurrentTeam(nullptr);
    engineKickoffNoTeam.runAIKickoffIfNeeded(kickoff_noTeam);
    BOOST_CHECK(true);

    Team teamA_kickoffNoAI(1, "Humans", 3);
    Team teamB_kickoffNoAI(2, "Orcs", 2);
    auto gamePtr_kickoffNoAI = std::make_shared<BloodBowlGame>(teamA_kickoffNoAI, teamB_kickoffNoAI);
    Kickoff* kickoff_noAI = dynamic_cast<Kickoff*>(gamePtr_kickoffNoAI->getStateList()[KICKOFF].get());

    Engine engineKickoffNoAI(gamePtr_kickoffNoAI);
    engineKickoffNoAI.runAIKickoffIfNeeded(kickoff_noAI);
    BOOST_CHECK(true);

    // runAIKickoffIfNeeded with secondAI
    Team teamA_kickoffSecondAI(1, "Humans", 3);
    Team teamB_kickoffSecondAI(2, "Orcs", 2);
    auto gamePtr_kickoffSecondAI = std::make_shared<BloodBowlGame>(teamA_kickoffSecondAI, teamB_kickoffSecondAI);
    Kickoff* kickoff_secondAI = dynamic_cast<Kickoff*>(gamePtr_kickoffSecondAI->getStateList()[KICKOFF].get());

    gamePtr_kickoffSecondAI->setCurrentTeam(&teamB_kickoffSecondAI);

    Engine engineKickoffSecondAI(gamePtr_kickoffSecondAI);
    auto mockAI_kickoffSecondAI = std::make_unique<MockAI>(engineKickoffSecondAI, gamePtr_kickoffSecondAI, 2);
    engineKickoffSecondAI.setSecondAI(std::move(mockAI_kickoffSecondAI));

    engineKickoffSecondAI.runAIKickoffIfNeeded(kickoff_secondAI);
    BOOST_CHECK(kickoff_secondAI->getTargetSelected());
}