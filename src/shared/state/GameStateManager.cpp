#include "GameStateManager.h"
#include "BloodBowlGame.h"
#include "Team.h"
#include "Character.h"
#include "TeamManager.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace state {

    GameStateManager* GameStateManager::instance = nullptr;

    GameStateManager::GameStateManager() {}

    GameStateManager& GameStateManager::getInstance() {
        if (instance == nullptr) {
            instance = new GameStateManager();
        }
        return *instance;
    }

    std::string GameStateManager::getSavesDirectory() const {
        return "saves/";
    }

    std::string GameStateManager::generateFilename(const std::string& saveName) const {
        // Créer un nom de fichier sûr
        std::string safe = saveName;
        for (char& c : safe) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                c = '_';
            }
        }

        // Ajouter un timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << safe << "_" << time_t << ".json";

        return getSavesDirectory() + ss.str();
    }

    bool GameStateManager::saveGame(const BloodBowlGame& game, const std::string& saveName) {
        // Créer le dossier saves s'il n'existe pas
        std::filesystem::create_directories(getSavesDirectory());

        std::string filename = generateFilename(saveName);

        Json::Value root = serializeGame(game);

        // Ajouter des métadonnées
        root["saveName"] = saveName;

        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        root["timestamp"] = ss.str();

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return false;
        }

        Json::StreamWriterBuilder writer;
        writer["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
        jsonWriter->write(root, &file);
        file.close();

        std::cout << "Game saved to " << filename << std::endl;
        return true;
    }

    Json::Value GameStateManager::serializeGame(const BloodBowlGame& game) {
        Json::Value root;

        // Données de jeu
        root["turnCounter"] = game.getTurnCounter();
        root["currentStateName"] = game.getCurrentState()->getName();

        // Position de la balle
        auto ballPos = game.getBallPosition();
        root["ballPosition"]["x"] = ballPos.first;
        root["ballPosition"]["y"] = ballPos.second;
        root["ballIsHold"] = game.getBallIsHold();

        // Équipe A
        Json::Value teamAJson;
        teamAJson["teamId"] = game.getTeamA().getTeamId();
        teamAJson["name"] = game.getTeamA().getName();
        teamAJson["score"] = game.getTeamA().getScore();
        teamAJson["rerolls"] = game.getTeamA().getRerolls();

        Json::Value charactersA(Json::arrayValue);
        for (const auto& character : game.getTeamA().getCharacters()) {
            Json::Value charJson;
            charJson["characterId"] = character->getId();
            charJson["name"] = character->getName();
            charJson["type"] = character->getType();
            charJson["movement"] = character->getMovement();
            charJson["strength"] = character->getStrength();
            charJson["agility"] = character->getAgility();
            charJson["armor"] = character->getArmor();
            charJson["status"] = static_cast<int>(character->getStatus());
            auto pos = character->getPosition();
            charJson["posX"] = pos.first;
            charJson["posY"] = pos.second;
            charJson["hasBall"] = character->getHasBall();
            charJson["gotUp"] = character->gotUp;
            charactersA.append(charJson);
        }
        teamAJson["characters"] = charactersA;
        root["teamA"] = teamAJson;

        // Équipe B
        Json::Value teamBJson;
        teamBJson["teamId"] = game.getTeamB().getTeamId();
        teamBJson["name"] = game.getTeamB().getName();
        teamBJson["score"] = game.getTeamB().getScore();
        teamBJson["rerolls"] = game.getTeamB().getRerolls();

        Json::Value charactersB(Json::arrayValue);
        for (const auto& character : game.getTeamB().getCharacters()) {
            Json::Value charJson;
            charJson["characterId"] = character->getId();
            charJson["name"] = character->getName();
            charJson["type"] = character->getType();
            charJson["movement"] = character->getMovement();
            charJson["strength"] = character->getStrength();
            charJson["agility"] = character->getAgility();
            charJson["armor"] = character->getArmor();
            charJson["status"] = static_cast<int>(character->getStatus());
            auto pos = character->getPosition();
            charJson["posX"] = pos.first;
            charJson["posY"] = pos.second;
            charJson["hasBall"] = character->getHasBall();
            charJson["gotUp"] = character->gotUp;
            charactersB.append(charJson);
        }
        teamBJson["characters"] = charactersB;
        root["teamB"] = teamBJson;

        // Équipe courante
        if (game.getCurrentTeam()) {
            root["currentTeamId"] = game.getCurrentTeam()->getTeamId();
        }

        return root;
    }

    std::shared_ptr<BloodBowlGame> GameStateManager::loadGame(const std::string& filename) {
        std::string fullPath = getSavesDirectory() + filename;

        std::ifstream file(fullPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for reading: " << fullPath << std::endl;
            return nullptr;
        }

        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errors;

        if (!Json::parseFromStream(reader, file, &root, &errors)) {
            std::cerr << "Failed to parse JSON: " << errors << std::endl;
            file.close();
            return nullptr;
        }
        file.close();

        return deserializeGame(root);
    }

    std::shared_ptr<BloodBowlGame> GameStateManager::deserializeGame(const Json::Value& root) {
        auto& teamManager = TeamManager::getInstance();

        // Charger équipe A
        const auto& teamAJson = root["teamA"];
        auto teamA = std::make_unique<Team>(
            teamAJson["teamId"].asInt(),
            teamAJson["name"].asString(),
            teamAJson["rerolls"].asInt()
        );
        teamA->setScore(teamAJson["score"].asInt());

        for (const auto& charJson : teamAJson["characters"]) {
            auto character = std::make_shared<Character>(
                charJson["characterId"].asInt(),
                charJson["name"].asString(),
                charJson["type"].asString(),
                charJson["movement"].asInt(),
                charJson["strength"].asInt(),
                charJson["agility"].asInt(),
                charJson["armor"].asInt()
            );

            // Restaurer l'état du match
            character->setStatus(static_cast<CharacterStatus>(charJson["status"].asInt()));
            character->setPosition({charJson["posX"].asInt(), charJson["posY"].asInt()});
            character->setHasBall(charJson["hasBall"].asBool());
            character->gotUp = charJson["gotUp"].asBool();

            teamA->addCharacter(character);
        }

        // Charger équipe B
        const auto& teamBJson = root["teamB"];
        auto teamB = std::make_unique<Team>(
            teamBJson["teamId"].asInt(),
            teamBJson["name"].asString(),
            teamBJson["rerolls"].asInt()
        );
        teamB->setScore(teamBJson["score"].asInt());

        for (const auto& charJson : teamBJson["characters"]) {
            auto character = std::make_shared<Character>(
                charJson["characterId"].asInt(),
                charJson["name"].asString(),
                charJson["type"].asString(),
                charJson["movement"].asInt(),
                charJson["strength"].asInt(),
                charJson["agility"].asInt(),
                charJson["armor"].asInt()
            );

            // Restaurer l'état du match
            character->setStatus(static_cast<CharacterStatus>(charJson["status"].asInt()));
            character->setPosition({charJson["posX"].asInt(), charJson["posY"].asInt()});
            character->setHasBall(charJson["hasBall"].asBool());
            character->gotUp = charJson["gotUp"].asBool();

            teamB->addCharacter(character);
        }

        // IMPORTANT: Sauvegarder les teamID pour récupérer les équipes après
        int teamAId = teamA->getTeamId();
        int teamBId = teamB->getTeamId();

        // Transférer la propriété des équipes au TeamManager
        // pour qu'elles restent en mémoire
        teamManager.saveTeam(std::move(teamA));
        teamManager.saveTeam(std::move(teamB));

        // Récupérer les pointeurs vers les équipes depuis le TeamManager
        Team* teamAPtr = teamManager.getTeamById(teamAId);
        Team* teamBPtr = teamManager.getTeamById(teamBId);

        if (!teamAPtr || !teamBPtr) {
            std::cerr << "Failed to retrieve teams from TeamManager!" << std::endl;
            return nullptr;
        }

        auto game = std::make_shared<BloodBowlGame>(*teamAPtr, *teamBPtr);

        // Restaurer l'état du jeu
        game->setTurnCounter(root["turnCounter"].asInt());
        game->setBallPosition({
            root["ballPosition"]["x"].asInt(),
            root["ballPosition"]["y"].asInt()
        });
        game->setBallIsHold(root["ballIsHold"].asBool());

        // Restaurer l'état courant
        std::string stateName = root["currentStateName"].asString();
        const auto& stateList = game->getStateList();

        if (stateName == "Setup") {
            game->setCurrentState(stateList.at(SETUP).get());
        } else if (stateName == "Kickoff") {
            game->setCurrentState(stateList.at(KICKOFF).get());
        } else if (stateName == "PlayerTurn") {
            game->setCurrentState(stateList.at(PLAYERTURN).get());
        } else if (stateName == "HalfTime") {
            game->setCurrentState(stateList.at(HALFTIME).get());
        } else if (stateName == "EndGame") {
            game->setCurrentState(stateList.at(ENDGAME).get());
        }

        // Restaurer l'équipe courante en utilisant le teamID
        if (root.isMember("currentTeamId")) {
            int currentTeamId = root["currentTeamId"].asInt();

            if (currentTeamId == teamAId) {
                game->setCurrentTeam(&game->getTeamA());
            } else if (currentTeamId == teamBId) {
                game->setCurrentTeam(&game->getTeamB());
            }
        }
        return game;
    }

    std::vector<SaveGameInfo> GameStateManager::listSavedGames() {
        std::vector<SaveGameInfo> saves;

        std::string savesDir = getSavesDirectory();

        if (!std::filesystem::exists(savesDir)) {
            return saves;
        }

        for (const auto& entry : std::filesystem::directory_iterator(savesDir)) {
            if (entry.path().extension() == ".json") {
                std::ifstream file(entry.path());
                if (file.is_open()) {
                    Json::Value root;
                    Json::CharReaderBuilder reader;
                    std::string errors;

                    if (Json::parseFromStream(reader, file, &root, &errors)) {
                        SaveGameInfo info;
                        info.filename = entry.path().filename().string();
                        info.displayName = root["saveName"].asString();
                        info.teamAId = root["teamA"]["teamId"].asInt();
                        info.teamAName = root["teamA"]["name"].asString();
                        info.teamBId = root["teamB"]["teamId"].asInt();
                        info.teamBName = root["teamB"]["name"].asString();
                        info.scoreA = root["teamA"]["score"].asInt();
                        info.scoreB = root["teamB"]["score"].asInt();
                        info.turnCounter = root["turnCounter"].asInt();
                        info.currentState = root["currentStateName"].asString();
                        info.timestamp = root["timestamp"].asString();

                        saves.push_back(info);
                    }
                    file.close();
                }
            }
        }

        return saves;
    }

    bool GameStateManager::deleteSave(const std::string& filename) {
        std::string fullPath = getSavesDirectory() + filename;

        try {
            return std::filesystem::remove(fullPath);
        } catch (const std::exception& e) {
            std::cerr << "Failed to delete save: " << e.what() << std::endl;
            return false;
        }
    }

    bool GameStateManager::saveExists(const std::string& filename) {
        std::string fullPath = getSavesDirectory() + filename;
        return std::filesystem::exists(fullPath);
    }

} // namespace state

