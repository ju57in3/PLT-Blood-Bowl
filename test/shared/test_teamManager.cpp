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

    auto cB1 = std::make_shared<Character>(1,"O1", "Orc", 6, 3, 3, 8);
    auto cB2 = std::make_shared<Character>(2,"O2", "Orc", 6, 3, 3, 8);
    teamB->addCharacter(cB1);
    teamB->addCharacter(cB2);

    teamManager.saveTeam(std::move(teamA));
    teamManager.saveTeam(std::move(teamB));

    BOOST_CHECK_EQUAL(teamManager.getNextTeamId(), 3);

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

    // Test deleteTeam
    BOOST_CHECK(teamManager.deleteTeam(1));
    BOOST_CHECK(!teamManager.deleteTeam(42));


    // Test updateTeam
    BOOST_CHECK(teamManager.updateTeam(2, "Gamma", 5));
    Team* t = teamManager.getTeamById(2);
    BOOST_REQUIRE(t != nullptr);
    BOOST_CHECK_EQUAL(t->getName(), "Gamma");
    BOOST_CHECK_EQUAL(t->getRerolls(), 5);

    BOOST_CHECK(!teamManager.updateTeam(42, "Delta", 1));

    // Test saveToDisk et loadFromDisk
    std::string filePath = "test_teams.json";

    // Sauvegarde
    BOOST_CHECK(teamManager.saveToDisk(filePath));

    // Tentative de sauvegarde dans un répertoire invalide
    BOOST_CHECK(!teamManager.saveToDisk("/invalid/path/teams.json"));

    // Chargement
    teamManager.clearTeams();
    BOOST_CHECK_EQUAL(teamManager.getTeamCount(), 0);
    BOOST_CHECK(teamManager.loadFromDisk(filePath));
    BOOST_CHECK_EQUAL(teamManager.getTeamCount(), 1);

    // Tentative de chargement depuis un fichier inexistant
    BOOST_CHECK(!teamManager.loadFromDisk("/invalid/path/file.json"));


    // Test getTeamCount
    BOOST_CHECK_EQUAL(teamManager.getTeamCount(), 1);
    teamManager.clearTeams();
    BOOST_CHECK_EQUAL(teamManager.getTeamCount(), 0);

}