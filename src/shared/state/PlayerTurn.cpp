#include "PlayerTurn.h"

#include "BloodBowlGame.h"
#include "Team.h"

namespace state {

    PlayerTurn::PlayerTurn(BloodBowlGame* game) : AbstractState(game) {
        isBlitzAvailable = true;
        isTurnOver = false;
        isTouchDown = false;
        if (game->turnCounter %2 == 0) {
            teamId = game->team1.getId();
            availableCharacter = game->team1.getActivePlayers();
        } else {
            teamId = game->team2.getId();
            availableCharacter = game->team2.getActivePlayers();
        }
    }

}
