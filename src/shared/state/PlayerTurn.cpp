#include "PlayerTurn.h"

#include "BloodBowlGame.h"
#include "EndGame.h"
#include "HalfTime.h"
#include "Setup.h"
#include "Team.h"

namespace state {
    PlayerTurn::PlayerTurn(BloodBowlGame* game) : AbstractState(game) {
        isBlitzAvailable = true;
        isTurnOver = false;
        isTouchDown = false;
        endTurn = false;
        teamId = game->getCurrentTeam().getTeamId();

        availableCharacters = game->getCurrentTeam().getPlayableCharacter();
    }

    void PlayerTurn::update() {
        if (game->getTurnCounter() == 16) {
            if (isTurnOver) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(&game->getStateList().at(HALFTIME));
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(&game->getStateList().at(HALFTIME));
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(&game->getStateList().at(HALFTIME));
            }
        } else if (game->getTurnCounter() == 32) {
            if (isTurnOver) {
                restoreCharactersStatus();
                switchTeam();
                game->setCurrentState(&game->getStateList().at(ENDGAME));
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                restoreCharactersStatus();
                switchTeam();
                game->setCurrentState(&game->getStateList().at(ENDGAME));
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(&game->getStateList().at(ENDGAME));
            }
        } else {
            if (isTurnOver) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(&game->getStateList().at(PLAYERTURN));
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(&game->getStateList().at(PLAYERTURN));
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(&game->getStateList().at(PLAYERTURN));
            }
        }

    }

    void PlayerTurn::restoreCharactersStatus() {
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


    void PlayerTurn::switchTeam() {
        if (game->getCurrentTeam().getTeamId() == game->getTeamA().getTeamId()) {
            game->setCurrentTeam(game->getTeamB());
        } else {
            game->setCurrentTeam(game->getTeamA());
        }
    }
}
