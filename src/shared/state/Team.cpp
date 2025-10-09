#include "Team.h"
#include "Character.h"

namespace state {
    std::vector<Character> Team::getCharacters() {
        return characters;
    }

    int Team::getScore(){
        return score;
    }

    std::vector<Character> Team::getPlayableCharacter()
    {
        std::vector<Character> characters;
        for (int i = 0; i < characters.size(); i++)
        {
            if (characters[i].getStatus() == playable)
            {
                characters.push_back(characters[i]);
            }
        }
        return characters;
    }
}