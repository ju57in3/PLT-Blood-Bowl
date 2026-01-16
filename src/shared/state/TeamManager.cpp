#include "TeamManager.h"
#include "Team.h"
#include "Character.h"
#include <algorithm>
#include <fstream>
#include <json/json.h>
#include <iostream>

namespace state {

    TeamManager* TeamManager::instance = nullptr;

    TeamManager::TeamManager() {

    }

    TeamManager& TeamManager::getInstance() {
        if (instance == nullptr) {
            instance = new TeamManager();
        }
        return *instance;
    }

    void TeamManager::saveTeam(std::unique_ptr<Team> team) {
        if (!team) return;

        // Vérifier si une équipe avec le même ID existe déjà
        auto it = std::find_if(savedTeams.begin(), savedTeams.end(),
            [&team](const std::unique_ptr<Team>& existingTeam) {
                return existingTeam->getTeamId() == team->getTeamId();
            });

        if (it != savedTeams.end()) {
            // Remplacer l'équipe existante
            std::cout << "[TeamManager] Replacing existing team ID " << team->getTeamId()
                      << " (" << (*it)->getName() << " -> " << team->getName() << ")" << std::endl;
            *it = std::move(team);
        } else {
            // Ajouter une nouvelle équipe
            std::cout << "[TeamManager] Adding new team ID " << team->getTeamId()
                      << " (" << team->getName() << ")" << std::endl;
            savedTeams.push_back(std::move(team));
        }
    }

    const std::vector<std::unique_ptr<Team>>& TeamManager::getSavedTeams() const {
        return savedTeams;
    }

    Team* TeamManager::getTeamById(int teamId) const {
        auto it = std::find_if(savedTeams.begin(), savedTeams.end(),
            [teamId](const std::unique_ptr<Team>& team) {
                return team->getTeamId() == teamId;
            });

        if (it != savedTeams.end()) {
            return it->get();
        }
        return nullptr;
    }

    Team* TeamManager::getTeamByName(const std::string& name) const {
        auto it = std::find_if(savedTeams.begin(), savedTeams.end(),
            [&name](const std::unique_ptr<Team>& team) {
                return team->getName() == name;
            });

        if (it != savedTeams.end()) {
            return it->get();
        }
        return nullptr;
    }

    void TeamManager::clearTeams() {
        savedTeams.clear();
    }

    int TeamManager::getTeamCount() const {
        return static_cast<int>(savedTeams.size());
    }

    bool TeamManager::deleteTeam(int teamId) {
        auto it = std::find_if(savedTeams.begin(), savedTeams.end(),
            [teamId](const std::unique_ptr<Team>& team) {
                return team->getTeamId() == teamId;
            });

        if (it != savedTeams.end()) {
            savedTeams.erase(it);
            return true;
        }
        return false;
    }

    bool TeamManager::updateTeam(int teamId, const std::string& newName, int newRerolls) {
        Team* team = getTeamById(teamId);
        if (team) {
            team->setName(newName);
            team->setRerolls(newRerolls);
            return true;
        }
        return false;
    }

    int TeamManager::getNextTeamId() const {
        int maxId = 0;
        for (const auto& team : savedTeams) {
            if (team->getTeamId() > maxId) {
                maxId = team->getTeamId();
            }
        }
        return maxId + 1;
    }

    bool TeamManager::saveToDisk(const std::string& filePath) {
        Json::Value root;
        root["teams"] = Json::Value(Json::arrayValue);

        for (const auto& team : savedTeams) {
            Json::Value teamJson;
            teamJson["teamId"] = team->getTeamId();
            teamJson["name"] = team->getName();
            teamJson["rerolls"] = team->getRerolls();
            teamJson["score"] = team->getScore();

            Json::Value charactersJson(Json::arrayValue);
            for (const auto& character : team->getCharacters()) {
                Json::Value charJson;
                charJson["characterId"] = character->getId();
                charJson["name"] = character->getName();
                charJson["type"] = character->getType();
                charJson["movement"] = character->getMovement();
                charJson["strength"] = character->getStrength();
                charJson["agility"] = character->getAgility();
                charJson["armor"] = character->getArmor();
                // Ne pas sauvegarder status et position - ce sont des données de partie
                charactersJson.append(charJson);
            }
            teamJson["characters"] = charactersJson;
            root["teams"].append(teamJson);
        }

        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return false;
        }

        Json::StreamWriterBuilder writer;
        writer["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
        jsonWriter->write(root, &file);
        file.close();

        std::cout << "Teams saved to " << filePath << std::endl;
        return true;
    }

    bool TeamManager::loadFromDisk(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for reading: " << filePath << std::endl;
            return false;
        }

        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errors;

        if (!Json::parseFromStream(reader, file, &root, &errors)) {
            std::cerr << "Failed to parse JSON: " << errors << std::endl;
            file.close();
            return false;
        }
        file.close();

        savedTeams.clear();

        if (!root.isMember("teams") || !root["teams"].isArray()) {
            std::cerr << "Invalid JSON format: 'teams' array not found" << std::endl;
            return false;
        }

        for (const auto& teamJson : root["teams"]) {
            int teamId = teamJson["teamId"].asInt();
            std::string name = teamJson["name"].asString();
            int rerolls = teamJson["rerolls"].asInt();

            auto team = std::make_unique<Team>(teamId, name, rerolls);

            if (teamJson.isMember("score")) {
                team->setScore(teamJson["score"].asInt());
            }

            if (teamJson.isMember("characters") && teamJson["characters"].isArray()) {
                for (const auto& charJson : teamJson["characters"]) {
                    auto character = std::make_shared<Character>(
                        charJson["characterId"].asInt(),
                        charJson["name"].asString(),
                        charJson["type"].asString(),
                        charJson["movement"].asInt(),
                        charJson["strength"].asInt(),
                        charJson["agility"].asInt(),
                        charJson["armor"].asInt()
                    );

                    // Ne pas charger status et position - ils seront initialisés lors du Setup

                    team->addCharacter(character);
                }
            }

            savedTeams.push_back(std::move(team));
        }

        std::cout << "Loaded " << savedTeams.size() << " teams from " << filePath << std::endl;
        return true;
    }

} // namespace state

