//
// Created by guilhem on 13/11/2025.
//
#include "Engine.h"

namespace engine{
    bool checkTouchDown(std::shared_ptr<state::BloodBowlGame> game)
    {
        if (!game->getBallIsHold())
        {
            return false;
        }

        if (game->getBallPosition().first < game->getWidth() || game->getBallPosition().second > 0)
        {
            return false;
        }

        std::vector<std::shared_ptr <state::Character>> charactersA = game->getTeamA().getCharacters();
        for ( int i=0; i < charactersA.size() ; i++)
        {
            std::shared_ptr <state::Character> characterA = charactersA[i];
            if (characterA->getHasBall() && game->getBallPosition().first > game->getWidth())
            {
                return true;
            }
        }

        std::vector<std::shared_ptr <state::Character>> charactersB = game->getTeamA().getCharacters();
        for ( int i=0; i < charactersB.size() ; i++)
        {
            std::shared_ptr <state::Character> characterB = charactersB[i];
            if (characterB->getHasBall() && game->getBallPosition().first > game->getWidth())
            {
                return true;
            }
        }
    }
}
