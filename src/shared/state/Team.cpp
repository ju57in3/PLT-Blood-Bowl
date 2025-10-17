#include "Team.h"
#include "Character.h"
#include <iostream>

namespace state {
    Team::Team(int teamId, std::string name, int rerolls)
        : teamId(teamId), name(name), rerolls(rerolls), score(0) {
    }

    std::vector<Character> Team::getCharacters() {
        return characters;
    }

    std::vector<Character> Team::getPlayableCharacter()
    {
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

    void Team::addCharacter(Character character) {
        characters.push_back(character);
    }

    std::ostream& operator<<(std::ostream& os, Team& team) {
        os << "Team ID: " << team.getTeamId() << "\n";
        os << "Score: " << team.getScore() << "\n";
        os << "Characters:\n";
        for (auto& character : team.getCharacters()) {
            os << character << "\n";
        }
        return os;
    }

    int Team::getTeamId() {
        return teamId;
    }

    std::string Team::getTeamName()
    {
        return name;
    }

    void Team::setTeamName(std::string name)
    {
        this->name = name;
    }

    int Team::getRerolls()
    {
        return rerolls;
    }

    void Team::setRerolls(int rerolls)
    {
        this->rerolls = rerolls;
    }

    int Team::getScore(){
        return score;
    }

    void Team::setScore(int score) {
        this->score = score;
    }

}
