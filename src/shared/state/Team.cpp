#include "Team.h"
#include "Character.h"
#include <iostream>

namespace state {
    std::vector<Character> Team::getCharacters() {
        return characters;
    }

    int Team::getScore(){
        return score;
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

    int Team::getTeamId() {
        return teamId;
    }

    void Team::setScore(int score) {
        this->score = score;
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
}
