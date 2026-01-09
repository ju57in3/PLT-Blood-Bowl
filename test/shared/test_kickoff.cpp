#include <boost/test/unit_test.hpp>
#include "state.h"
#include <cstdlib>

using namespace state;

BOOST_AUTO_TEST_CASE(TestKickoff)
{
    // Setup teams
    Team teamA(1, "Humans", 11);
    Team teamB(2, "Orcs", 11);

    // Adding characters for each team
    for (int i = 0; i < 11; i++) {
        auto cA = std::make_unique<Character>(1, "H" + std::to_string(i), "Human", 6, 3, 3, 8);
        cA->setStatus(playable);
        cA->setPosition({i, 5});
        teamA.addCharacter(std::move(cA));

        auto cB = std::make_unique<Character>(2, "O" + std::to_string(i), "Orcs", 5, 4, 2, 9);
        cB->setStatus(playable);
        cB->setPosition({15 + i, 5});
        teamB.addCharacter(std::move(cB));
    }

    // Creation of the game
    BloodBowlGame game(teamA, teamB);
    Kickoff* kickoff = dynamic_cast<Kickoff*>(game.getStateList()[KICKOFF].get());
    BOOST_REQUIRE(kickoff != nullptr);

    // Test getName()
    BOOST_CHECK_EQUAL(kickoff->getName(), "Kickoff");

    // Test setTarget and setTargetSelected
    std::pair<int, int> target = {10, 7};
    kickoff->setTarget(target);
    kickoff->setTargetSelected(true);

    // Test that targetSelected can be set to false
    kickoff->setTargetSelected(false);
    BOOST_CHECK(true); // No crash = success

    // Test isValidKickoffTarget for Team A (left half: 0-12)
    BOOST_CHECK(kickoff->isValidKickoffTarget({5, 5}, teamA));
    BOOST_CHECK(kickoff->isValidKickoffTarget({0, 0}, teamA));
    BOOST_CHECK(kickoff->isValidKickoffTarget({12, 14}, teamA));
    BOOST_CHECK(!kickoff->isValidKickoffTarget({13, 5}, teamA)); // Wrong half
    BOOST_CHECK(!kickoff->isValidKickoffTarget({20, 10}, teamA)); // Wrong half
    BOOST_CHECK(!kickoff->isValidKickoffTarget({-1, 5}, teamA)); // Out of bounds
    BOOST_CHECK(!kickoff->isValidKickoffTarget({5, -1}, teamA)); // Out of bounds
    BOOST_CHECK(!kickoff->isValidKickoffTarget({30, 5}, teamA)); // Out of bounds
    BOOST_CHECK(!kickoff->isValidKickoffTarget({5, 20}, teamA)); // Out of bounds

    // Test isValidKickoffTarget for Team B (right half: 13-25)
    BOOST_CHECK(kickoff->isValidKickoffTarget({15, 5}, teamB));
    BOOST_CHECK(kickoff->isValidKickoffTarget({13, 0}, teamB));
    BOOST_CHECK(kickoff->isValidKickoffTarget({25, 14}, teamB));
    BOOST_CHECK(!kickoff->isValidKickoffTarget({5, 5}, teamB)); // Wrong half
    BOOST_CHECK(!kickoff->isValidKickoffTarget({12, 10}, teamB)); // Wrong half
    BOOST_CHECK(!kickoff->isValidKickoffTarget({-1, 5}, teamB)); // Out of bounds
    BOOST_CHECK(!kickoff->isValidKickoffTarget({26, 5}, teamB)); // Out of bounds

    // Test boundary conditions for Team A
    BOOST_CHECK(kickoff->isValidKickoffTarget({0, 5}, teamA)); // minX
    BOOST_CHECK(kickoff->isValidKickoffTarget({12, 5}, teamA)); // maxX
    BOOST_CHECK(!kickoff->isValidKickoffTarget({13, 5}, teamA)); // maxX + 1

    // Test boundary conditions for Team B
    BOOST_CHECK(kickoff->isValidKickoffTarget({13, 5}, teamB)); // minX
    BOOST_CHECK(kickoff->isValidKickoffTarget({25, 5}, teamB)); // maxX
    BOOST_CHECK(!kickoff->isValidKickoffTarget({12, 5}, teamB)); // minX - 1

    // Test kickBall with valid target - since it's random, just verify it doesn't crash
    // and that the ball ends up somewhere valid
    std::pair<int, int> base = {10, 10};
    kickoff->kickBall(base);
    auto pos = game.getBallPosition();

    // Check if ball position stays on the field or assigned to a player
    BOOST_CHECK(pos.first >= 0 && pos.first < 26);
    BOOST_CHECK(pos.second >= 0 && pos.second < 15);

    // Test kickBall with invalid target (out of bounds for current team)
    // This should assign ball to closest player
    game.setCurrentTeam(&game.getTeamA());
    kickoff->kickBall({20, 10}); // Invalid for team A (wrong half)
    auto posInvalid = game.getBallPosition();
    BOOST_CHECK(posInvalid.first >= 0 && posInvalid.first < 26);
    BOOST_CHECK(posInvalid.second >= 0 && posInvalid.second < 15);

    // Test that one of team A's players has the ball
    bool someoneForgotBall = true;
    for (const auto& c : game.getTeamA().getCharacters()) {
        if (c && c->getHasBall()) {
            someoneForgotBall = false;
            // Ball position should match player position
            BOOST_CHECK_EQUAL(posInvalid.first, c->getPosition().first);
            BOOST_CHECK_EQUAL(posInvalid.second, c->getPosition().second);
            break;
        }
    }
    BOOST_CHECK(!someoneForgotBall);

    // Reset ball state
    for (auto& c : game.getTeamA().getCharacters()) {
        if (c) c->setHasBall(false);
    }

    // Test update() without targetSelected (should do nothing)
    game.setCurrentState(kickoff);
    game.setCurrentTeam(&game.getTeamA());
    auto stateBefore = game.getCurrentState();
    kickoff->setTargetSelected(false);
    kickoff->update();
    BOOST_CHECK(game.getCurrentState() == stateBefore);

    // Test update() when currentTeam = teamA with targetSelected
    game.setCurrentState(kickoff);
    game.setCurrentTeam(&game.getTeamA());
    kickoff->setTargetSelected(true);
    kickoff->setTarget({10, 5});
    kickoff->update();

    // After update() we should pass to PLAYERTURN
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[PLAYERTURN].get());

    // Verify that the ball has a valid position
    auto posAfterA = game.getBallPosition();
    BOOST_CHECK(posAfterA.first >= 0 && posAfterA.first < 26);
    BOOST_CHECK(posAfterA.second >= 0 && posAfterA.second < 15);

    // Reset ball state
    for (auto& c : game.getTeamA().getCharacters()) {
        if (c) c->setHasBall(false);
    }
    for (auto& c : game.getTeamB().getCharacters()) {
        if (c) c->setHasBall(false);
    }

    // Test update() when currentTeam = teamB - reset kickoff state first
    game.setCurrentState(kickoff);
    game.setCurrentTeam(&game.getTeamB());
    kickoff->setTargetSelected(true);
    kickoff->setTarget({15, 8});
    kickoff->update();

    auto posAfterB = game.getBallPosition();
    BOOST_CHECK(posAfterB.first >= 0 && posAfterB.first < 26);
    BOOST_CHECK(posAfterB.second >= 0 && posAfterB.second < 15);

    // Verify that the state stays in PLAYERTURN
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[PLAYERTURN].get());

    // Test kickBall multiple times to cover different random directions
    // (although we can't predict exact outcomes, we verify consistency)
    for (int i = 0; i < 20; i++) {
        game.setCurrentState(kickoff);
        game.setCurrentTeam(&game.getTeamA());

        // Reset ball state
        for (auto& c : game.getTeamA().getCharacters()) {
            if (c) c->setHasBall(false);
        }
        for (auto& c : game.getTeamB().getCharacters()) {
            if (c) c->setHasBall(false);
        }

        kickoff->kickBall({10, 7});
        auto testPos = game.getBallPosition();

        // Ball should always end up in valid position
        BOOST_CHECK(testPos.first >= 0 && testPos.first < 26);
        BOOST_CHECK(testPos.second >= 0 && testPos.second < 15);
    }

    // Test edge case: kickBall to corners
    std::vector<std::pair<int, int>> cornerTargets = {
        {0, 0}, {0, 14}, {12, 0}, {12, 14}  // Team A valid corners
    };

    game.setCurrentTeam(&game.getTeamA());
    for (const auto& corner : cornerTargets) {
        // Reset ball state
        for (auto& c : game.getTeamA().getCharacters()) {
            if (c) c->setHasBall(false);
        }

        kickoff->kickBall(corner);
        auto cornerPos = game.getBallPosition();
        BOOST_CHECK(cornerPos.first >= 0 && cornerPos.first < 26);
        BOOST_CHECK(cornerPos.second >= 0 && cornerPos.second < 15);
    }

    // Test that kickoff doesn't crash with target at board edges
    std::vector<std::pair<int, int>> edgeTargets = {
        {0, 7}, {12, 7}, {6, 0}, {6, 14}
    };

    for (const auto& edge : edgeTargets) {
        // Reset ball state
        for (auto& c : game.getTeamA().getCharacters()) {
            if (c) c->setHasBall(false);
        }

        kickoff->kickBall(edge);
        auto edgePos = game.getBallPosition();
        BOOST_CHECK(edgePos.first >= 0 && edgePos.first < 26);
        BOOST_CHECK(edgePos.second >= 0 && edgePos.second < 15);
    }
}