#include <boost/test/unit_test.hpp>
#include "state.h"
#include <cstdlib>

using namespace state;

BOOST_AUTO_TEST_CASE(TestKickoff)
{
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    // Adding characters for each team
    for (int i = 0; i < 3; i++) {
        auto cA = std::make_unique<Character>(1,"H1", "Human", 6, 3, 3, 8);
        cA->setStatus(playable);
        cA->setPosition({5,5});
        teamA.addCharacter(std::move(cA));

        auto cB = std::make_unique<Character>(2,"O1", "Orcs", 5, 4, 2, 9);
        cB->setStatus(playable);
        cB->setPosition({20,5});
        teamB.addCharacter(std::move(cB));
    }


    // Creation of the game
    BloodBowlGame game(teamA, teamB);
    Kickoff* kickoff = dynamic_cast<Kickoff*>(game.getStateList()[KICKOFF].get());
    BOOST_REQUIRE(kickoff != nullptr);

    //Test of KickBall - since it's random, just verify it doesn't crash
    // and that the ball ends up somewhere valid
    std::pair<int, int> base = {10, 10};
    kickoff->kickBall(base);
    auto pos = game.getBallPosition();

    // Check if ball position stay on the field or assigned to a player
    BOOST_CHECK(pos.first >= 0 && pos.first < 26);
    BOOST_CHECK(pos.second >= 0 && pos.second < 15);

    // Test of update() when currentTeam = teamA
    game.setCurrentTeam(&game.getTeamA());
    kickoff->setTargetSelected(true);
    kickoff->setTarget({10, 5});
    kickoff->update();

    // After update() we should pass to PLAYERTURN
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[PLAYERTURN].get());

    // Verify that the ball has a valid position
    auto posAfterA = game.getBallPosition();
    BOOST_CHECK(posAfterA.first >= 0 && posAfterA.first < 26);

    // Test update() when currentTeam = teamB - reset kickoff state first
    game.setCurrentState(kickoff);
    game.setCurrentTeam(&game.getTeamB());
    kickoff->setTargetSelected(true);
    kickoff->setTarget({15, 8});
    kickoff->update();

    auto posAfterB = game.getBallPosition();
    BOOST_CHECK(posAfterB.first >= 0 && posAfterB.first < 26);

    //Verify that the state stay in PLAYERTURN
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[PLAYERTURN].get());
}