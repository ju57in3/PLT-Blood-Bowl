#include "EndGame.h"
#include "Team.h"
#include "BloodBowlGame.h"

namespace state {
    EndGame::EndGame(BloodBowlGame* game):AbstractState(game){}

    void update()
    {
        if (restart == true)
        {
            Team teamA = game.getTeamA();
            game->setCurrentState(new );
        }
    }

}