//
// Created by justine on 19/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"
#include <sstream>

using namespace state;

BOOST_AUTO_TEST_CASE(TestBloodBowlGame)
{
    // Creation of the teams
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    // Adding characters in each team
    for (int i = 0; i < 3; i++) {
        auto h = std::make_unique<Character>("Human" + std::to_string(i+1), "Human", 6, 3, 3, 8);
        h->setStatus(playable);
        h->setPosition({13, i});
        teamA.addCharacter(std::move(h));


        auto o = std::make_unique<Character>("Orc" + std::to_string(i+1), "Orc", 5, 3, 2, 9);
        o->setStatus(playable);
        o->setPosition({2, i});
        teamB.addCharacter(std::move(o));
    }

    // Creation of the game
    BloodBowlGame game(teamA, teamB);

    // Check the teams registration
    BOOST_CHECK_EQUAL(game.getTeamA().getName(), "Humans");
    BOOST_CHECK_EQUAL(game.getTeamB().getName(), "Orcs");

    // Check the states
    const auto& stateList = game.getStateList();
    BOOST_CHECK(!stateList.empty());
    BOOST_CHECK_GE(stateList.size(), 5); // SETUP, KICKOFF, PLAYERTURN, HALFTIME, ENDGAME

    BOOST_CHECK(game.getCurrentState() != nullptr);
    BOOST_CHECK(game.getCurrentState() == stateList[SETUP].get());
    BOOST_CHECK(game.getCurrentState() == stateList[KICKOFF].get());
    BOOST_CHECK(game.getCurrentState() == stateList[PLAYERTURN].get());
    BOOST_CHECK(game.getCurrentState() == stateList[HALFTIME].get());
    BOOST_CHECK(game.getCurrentState() == stateList[ENDGAME].get());

    // Check the draw
    Team* coinWinner = game.coinToss();
    int winnerId = coinWinner->getTeamId();
    BOOST_CHECK((winnerId == 1) || (winnerId == 2));

    // Check the counter
    BOOST_CHECK_EQUAL(game.getTurnCounter(), 0);
    game.setTurnCounter(4);
    BOOST_CHECK_EQUAL(game.getTurnCounter(), 4);

    // Check the ball position
    game.setBallPosition({5, 6});
    auto pos = game.getBallPosition();
    BOOST_CHECK_EQUAL(pos.first, 5);
    BOOST_CHECK_EQUAL(pos.second, 6);

    // Check the land height and width
    int width = game.getWidth();
    int height = game.getHeight();
    BOOST_CHECK_GT(width, 0);
    BOOST_CHECK_GT(height, 0);

    // Check the current team
    Team* current = game.getCurrentTeam();
    BOOST_CHECK(current->getTeamId() == 1 || current->getTeamId() == 2);

    game.setCurrentTeam(&teamB);
    BOOST_CHECK_EQUAL(game.getCurrentTeam()->getName(), "Orcs");

    // Check the number of repetition
    game.setNbRepetition(3);
    BOOST_CHECK_EQUAL(game.getNbRepetition(), 3);

    // State change test
    game.setCurrentState(stateList[KICKOFF].get());
    BOOST_CHECK(game.getCurrentState() == stateList[KICKOFF].get());

    // Display test
    std::ostringstream oss;
    oss << game;
    std::string output = oss.str();
    BOOST_CHECK(output.find("Team") != std::string::npos);
    BOOST_CHECK(output.find("Turn") != std::string::npos);

    // Overall consistency
    BOOST_CHECK_EQUAL(game.getTeamA().getCharacters().size(), 3);
    BOOST_CHECK_EQUAL(game.getTeamB().getCharacters().size(), 3);
}