#include "EndGame.h"
#include "Team.h"
#include "BloodBowlGame.h"
#include "Setup.h"

namespace state {
    EndGame::EndGame(BloodBowlGame* game):AbstractState(game){}

    void EndGame::update()
    {
        if (restart == true)
        {
            for (auto character : game->getTeamA().getCharacters())
            {
                character.setStatus(bench);
            }
            for (auto character : game->getTeamB().getCharacters())
            {
                character.setStatus(bench);
            }
            new BloodBowlGame(game->getTeamA(), game->getTeamB());
        }
    }

}
