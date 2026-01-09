#include "TeamManager.h"
#include "Team.h"
#include <algorithm>

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
        if (team) {
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

} // namespace state

