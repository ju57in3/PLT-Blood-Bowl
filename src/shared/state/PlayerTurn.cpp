#include "PlayerTurn.h"

#include "BloodBowlGame.h"
#include "Team.h"

namespace state {

    PlayerTurn::PlayerTurn(BloodBowlGame* game) : AbstractState(game) {
        isBlitzAvailable = true;
        isTurnOver = false;
        isTouchDown = false;
        teamId = game->getCurrentTeam().getTeamId();

        availableCharacters = game->getCurrentTeam().getPlayableCharacter();
    }

    void update() {
        // To be implemented
    }

    void restoreCharactersStatus() {

    }
}
