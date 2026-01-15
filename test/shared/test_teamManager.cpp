//
// Created by matt-o on 1/15/26.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestEndGame) {
    // Initialisation of teams and Team Manager
    auto& teamManager = TeamManager::getInstance();

    auto teamA = std::make_unique<Team>(1, "Team A", 3);
    auto teamB = std::make_unique<Team>(2, "Team B", 3);

    auto cA1 = std::make_shared<Character>(1,"H1", "Human", 6, 3, 3, 8);
    auto cA2 = std::make_shared<Character>(2,"H2", "Human", 6, 3, 3, 8);
    auto cB1 = std::make_shared<Character>(1,"O1", "Orc", 5, 3, 3, 9);

    cA1->setStatus(playable);
    cA2->setStatus(playable);
    cB1->setStatus(playable);

    teamA->addCharacter(cA1);
    teamA->addCharacter(cA2);
    teamB->addCharacter(cB1);

    teamManager.saveTeam(std::move(teamA));
    teamManager.saveTeam(std::move(teamB));

    // Test for getSavedTeam
    const auto& teams = teamManager.getSavedTeams();

    BOOST_CHECK_EQUAL(teams.size(), 2);
    BOOST_CHECK(teams[0] != nullptr);

    // Test of getTeamBy...
    Team* team = teamManager.getTeamById(2);
    BOOST_CHECK(team != nullptr);
    BOOST_TEST(team->getTeamId() == 2);
    BOOST_TEST(team->getName() == "Team B");
    team = teamManager.getTeamById(0);
    BOOST_CHECK(team == nullptr);

    Team* team2 = teamManager.getTeamByName("Team A");
    BOOST_CHECK(team2 != nullptr);
    BOOST_TEST(team2->getTeamId() == 1);
    BOOST_TEST(team2->getName() == "Team A");
    team2 = teamManager.getTeamByName("Test");
    BOOST_CHECK(team2 == nullptr);

    // Test getTeamCount
    BOOST_CHECK_EQUAL(teamManager.getTeamCount(), 2);
    teamManager.clearTeams();
    BOOST_CHECK_EQUAL(teamManager.getTeamCount(), 0);
}