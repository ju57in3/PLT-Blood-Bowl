//
// Created by justine on 16/10/2025.
//

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
        auto cA = std::make_unique<Character>("H1", "Human", 6, 3, 3, 8);
        cA->setStatus(playable);
        cA->setPosition({5,5});
        teamA.addCharacter(std::move(cA));

        auto cB = std::make_unique<Character>("O1", "Orcs", 5, 4, 2, 9);
        cB->setStatus(playable);
        cB->setPosition({20,5});
        teamB.addCharacter(std::move(cB));
    }


    // Creation of the game
    BloodBowlGame game(teamA, teamB);
    Kickoff kickoff(&game);

    //Test of KickBall on 8 directions
    std::pair<int, int> base = {10, 10};
    for (int dir = 0; dir < 8; dir++) {
        srand(dir);                      //force rand() to a reproductable result
        kickoff.kickBall(base);
        auto pos = game.getBallPosition();

        // Check if ball position has changed
        BOOST_CHECK((pos.first != base.first) || (pos.second != base.second));

        // Check if ball position stay on the field
        BOOST_CHECK(pos.first >=0);
        BOOST_CHECK(pos.second >=0);
        BOOST_CHECK(pos.second <= game.getHeight());
    }

    // Test of update() when currentTeam = teamA
    srand(1);        //force rand() for reproductibility
    game.setCurrentTeam(&game.getTeamA());
    kickoff.update();

    // After update() we should pass to PLAYERTURN
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[PLAYERTURN].get());

    // Verify that the ball has a valid position
    auto posAfterA = game.getBallPosition();
    BOOST_CHECK(posAfterA.first >= 1 && posAfterA.first <= 13);
    BOOST_CHECK(posAfterA.second >= 0 && posAfterA.second < game.getHeight());

    // Test update() when currentTeam = teamB
    srand(2);
    game.setCurrentTeam(&game.getTeamB());
    kickoff.update();

    auto posAfterB = game.getBallPosition();
    BOOST_CHECK(posAfterB.first >= 12 && posAfterB.first <= 24);
    BOOST_CHECK(posAfterB.second >= 0 && posAfterB.second < game.getHeight());

    //Verify that the state stay in PLAYERTURN
    BOOST_CHECK(game.getCurrentState() == game.getStateList()[PLAYERTURN].get());
}