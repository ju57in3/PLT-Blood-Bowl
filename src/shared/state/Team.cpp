#include "Team.h"
#include "Character.h"
#include <iostream>
#include "constants.h"

namespace state {
    Team::Team(int teamId, std::string name, int rerolls)
        : teamId(teamId), name(std::move(name)), rerolls(rerolls), score(0) {
        characters.reserve(TEAM_MAX_PLAYERS);
    }

    std::vector<std::shared_ptr<Character>>& Team::getCharacters() {
        return characters;
    }

    const std::vector<std::shared_ptr<Character>>& Team::getCharacters() const {
        return characters;
    }

    int Team::getScore() const {
        return score;
    }

    std::vector<Character*> Team::getPlayableCharacter() {
        std::vector<Character*> playableChars;
        for (auto & characterPtr : characters)
        {
            if (characterPtr->getStatus() == playable)
            {
                playableChars.push_back(characterPtr.get());
            }
        }
        return playableChars;
    }

    std::vector<const Character*> Team::getPlayableCharacter() const {
        std::vector<const Character*> playableChars;
        for (const auto & characterPtr : characters)
        {
            if (characterPtr->getStatus() == playable)
            {
                playableChars.push_back(characterPtr.get());
            }
        }
        return playableChars;
    }

    int Team::getTeamId() const{
        return teamId;
    }

    void Team::setScore(int score) {
        this->score = score;
    }

    void Team::addCharacter(std::shared_ptr<Character> character) {
        if (characters.size() >= TEAM_MAX_PLAYERS) {
            std::cerr << "Team " << name << " is full (" << TEAM_MAX_PLAYERS << " players max). Character not added." << std::endl;
            return;
        }
        characters.push_back(std::move(character));
    }

    std::ostream& operator<<(std::ostream& os, const Team& team) {
        os << "Team ID: " << team.getTeamId() << "\n";
        os << "Score: " << team.getScore() << "\n";
        os << "Characters:\n";
        for (const auto& characterPtr : team.getCharacters()) {
            os << *characterPtr << "\n";
        }
        return os;
    }

    std::string Team::getName() const
    {
        return name;
    }

    void Team::setName(std::string teamName)
    {
        this->name = teamName;
    }

    int Team::getRerolls() const
    {
        return rerolls;
    }

    void Team::setRerolls(int rerollsValue)
    {
        this->rerolls = rerollsValue;
    }
}
