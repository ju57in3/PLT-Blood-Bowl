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
        std::vector<Character> pieces;
        for (int i = 0; i < pieces.size(); i++)
        {
            if (pieces[i].getStatus() == playable)
            {
                pieces.push_back(pieces[i]);
            }
        }
        return pieces;
    }
}