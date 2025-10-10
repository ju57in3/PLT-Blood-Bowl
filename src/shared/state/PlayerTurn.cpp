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

    }

    void PlayerTurn::restoreCharacterStatus() {
        std::vector<Character> characters = game->getCurrentTeam().getCharacters();
        for (int i = 0; i < characters.size(); i++)
        {
            if (characters[i].getStatus() == played)
            {
                characters[i].setStatus(playable);
            }
            else if (characters[i].getStatus() == stunned)
            {
                characters[i].setStatus(knockedDown);
            }
        }
    }
}
