//
// Created by matt-o on 1/19/26.
//

#include <filesystem>
#include <fstream>
#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

BOOST_AUTO_TEST_CASE(TestGameStateManager){
    // ===== SETUP: Nettoyer avant de commencer =====
    if (std::filesystem::exists("saves/")) {
        std::filesystem::remove_all("saves/");
    }

    BOOST_CHECK(!std::filesystem::exists("saves/"));

    // Init
    auto& manager = GameStateManager::getInstance();

    auto saves = manager.listSavedGames();
    BOOST_CHECK_EQUAL(saves.size(), 0);

    auto teamA = std::make_unique<Team>(1, "Team A", 3);
    auto teamB = std::make_unique<Team>(2, "Team B", 3);

    auto charA = std::make_shared<Character>(1, "Player1", "Human", 6, 3, 3, 8);
    charA->setPosition({5, 5});
    charA->setStatus(playable);
    teamA->addCharacter(charA);
    teamA->setScore(2);

    auto charB = std::make_shared<Character>(2, "Player2", "Orc", 6, 3, 3, 7);
    charB->setPosition({10, 10});
    charB->setStatus(played);
    charB->setHasBall(true);
    charB->gotUp = true;
    teamB->addCharacter(charB);
    teamB->setScore(1);

    auto gamePtr = std::make_shared<BloodBowlGame>(*teamA, *teamB);
    gamePtr->setTurnCounter(5);
    gamePtr->setBallPosition({7, 8});
    gamePtr->setBallIsHold(true);

    // Test simple save
    bool result = manager.saveGame(*gamePtr, "test "); // Test with a space
    BOOST_CHECK(result);
    BOOST_CHECK(std::filesystem::exists("saves/"));

    saves = manager.listSavedGames();
    BOOST_CHECK_EQUAL(saves.size(), 1);

    std::ifstream file("saves/" + saves[0].filename);
    BOOST_REQUIRE(file.is_open());

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    BOOST_REQUIRE(Json::parseFromStream(reader, file, &root, &errors)); // Parse the files, if error, stop
    file.close();  // Fermer le fichier après lecture

    // Checking all datas :
    BOOST_CHECK_EQUAL(root["turnCounter"].asInt(), 5);
    BOOST_CHECK_EQUAL(root["ballPosition"]["x"].asInt(), 7);
    BOOST_CHECK_EQUAL(root["ballPosition"]["y"].asInt(), 8);
    BOOST_CHECK_EQUAL(root["ballIsHold"].asBool(), true);

    BOOST_CHECK_EQUAL(root["teamA"]["teamId"].asInt(), 1);
    BOOST_CHECK_EQUAL(root["teamA"]["name"].asString(), "Team A");
    BOOST_CHECK_EQUAL(root["teamA"]["score"].asInt(), 2);
    BOOST_CHECK_EQUAL(root["teamA"]["rerolls"].asInt(), 3);
    BOOST_CHECK_EQUAL(root["teamA"]["characters"].size(), 1);

    const auto& char1 = root["teamA"]["characters"][0];
    BOOST_CHECK_EQUAL(char1["characterId"].asInt(), 1);
    BOOST_CHECK_EQUAL(char1["name"].asString(), "Player1");
    BOOST_CHECK_EQUAL(char1["type"].asString(), "Human");
    BOOST_CHECK_EQUAL(char1["posX"].asInt(), 5);
    BOOST_CHECK_EQUAL(char1["posY"].asInt(), 5);

    BOOST_CHECK_EQUAL(root["teamB"]["teamId"].asInt(), 2);
    BOOST_CHECK_EQUAL(root["teamB"]["name"].asString(), "Team B");
    BOOST_CHECK_EQUAL(root["teamB"]["score"].asInt(), 1);

    const auto& char2 = root["teamB"]["characters"][0];
    BOOST_CHECK_EQUAL(char2["hasBall"].asBool(), true);
    BOOST_CHECK_EQUAL(char2["gotUp"].asBool(), true);

    BOOST_CHECK_EQUAL(root["saveName"].asString(), "test ");
    BOOST_CHECK(!root["timestamp"].asString().empty());
    BOOST_CHECK_EQUAL(root["currentTeamId"].asInt(), 1);

    // Test loadGame
    auto loadedGame = manager.loadGame(saves[0].filename);
    BOOST_REQUIRE(loadedGame != nullptr);

    // Checking datas are correctly loaded
    BOOST_CHECK_EQUAL(loadedGame->getTurnCounter(), 5);
    BOOST_CHECK_EQUAL(loadedGame->getBallPosition().first, 7);
    BOOST_CHECK_EQUAL(loadedGame->getBallPosition().second, 8);
    BOOST_CHECK_EQUAL(loadedGame->getBallIsHold(), true);

    BOOST_CHECK_EQUAL(loadedGame->getTeamA().getName(), "Team A");
    BOOST_CHECK_EQUAL(loadedGame->getTeamA().getScore(), 2);
    BOOST_CHECK_EQUAL(loadedGame->getTeamB().getName(), "Team B");
    BOOST_CHECK_EQUAL(loadedGame->getTeamB().getScore(), 1);

    // Test loadGame avec fichier inexistant
    auto gameNotFound = manager.loadGame("nonexistent_file.json");
    BOOST_CHECK(gameNotFound == nullptr);

    // Créer invalid json
    std::ofstream fileIncorrect("saves/invalid.json");
    fileIncorrect << "{ invalid json content }}}";
    fileIncorrect.close();

    auto gameInvalid = manager.loadGame("invalid.json");
    BOOST_CHECK(gameInvalid == nullptr);

    // Test deserialization with every game states and modification
    std::vector<std::string> states = {"Setup", "Kickoff", "PlayerTurn", "HalfTime", "EndGame"};

    for (const auto& stateName : states) {
        // Save a new game for every states
        manager.saveGame(*gamePtr, "state_test_" + stateName);

        saves = manager.listSavedGames();
        auto it = std::find_if(saves.begin(), saves.end(),
            [&stateName](const SaveGameInfo& s) {
                return s.displayName == "state_test_" + stateName;
            });

        if (it != saves.end()) {
            // Modify the state in the json file
            std::string filename = "saves/" + it->filename;
            std::ifstream inFile(filename);

            Json::Value rootState;
            if (Json::parseFromStream(reader, inFile, &rootState, &errors)) {
                inFile.close();

                rootState["currentStateName"] = stateName;

                std::ofstream outFile(filename);
                Json::StreamWriterBuilder writer;
                std::unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
                jsonWriter->write(rootState, &outFile);
                outFile.close();

                // Load and check is it has been modify
                auto gameWithState = manager.loadGame(it->filename);
                BOOST_CHECK(gameWithState != nullptr);
                if (gameWithState) {
                    BOOST_CHECK_EQUAL(gameWithState->getCurrentState()->getName(), stateName);
                }
            }
        }
    }

    // Test saveExists
    saves = manager.listSavedGames();
    if (!saves.empty()) {
        bool exists = manager.saveExists(saves[0].filename);
        BOOST_CHECK(exists);

        // Test deleteSave
        bool deleted = manager.deleteSave(saves[0].filename);
        BOOST_CHECK(deleted);
    }

    bool notExists = manager.saveExists("nonexistent.json");
    BOOST_CHECK(!notExists);

    bool notDeleted = manager.deleteSave("nonexistent.json");
    BOOST_CHECK(!notDeleted);

    // Cleaning
    std::filesystem::remove_all("saves/");
    BOOST_CHECK(!std::filesystem::exists("saves/"));
}