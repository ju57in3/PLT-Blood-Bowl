#include "Team.h"
#include "Character.h"
#include <iostream>

namespace state {
    Team::Team(int teamId, std::string name, int rerolls)
        : teamId(teamId), name(std::move(name)), rerolls(rerolls), score(0) {
    }

    std::vector<Character> Team::getCharacters() const {
        return characters;
    }

    int Team::getScore() const {
        return score;
    }

    std::vector<Character> Team::getPlayableCharacter() const {
        std::vector<Character> playableCharacters;
        for (auto & character : characters)
        {
            if (character.getStatus() == playable)
            {
                playableCharacters.push_back(character);
            }
        }
        return playableCharacters;
    }

    int Team::getTeamId() const{
        return teamId;
    }

    void Team::setScore(int score) {
        this->score = score;
    }

    void Team::addCharacter(const Character& character) {
        characters.push_back(character);
    }

    std::ostream& operator<<(std::ostream& os, const Team& team) {
        os << "Team ID: " << team.getTeamId() << "\n";
        os << "Score: " << team.getScore() << "\n";
        os << "Characters:\n";
        for (const auto& character : team.getCharacters()) {
            os << character << "\n";
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
