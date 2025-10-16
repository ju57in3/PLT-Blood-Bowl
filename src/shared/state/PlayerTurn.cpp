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
                game->setCurrentState(game->getStateList().at(HALFTIME).get());
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(HALFTIME).get());
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(HALFTIME).get());
            }
        } else if (game->getTurnCounter() == 32) {
            if (isTurnOver) {
                restoreCharactersStatus();
                switchTeam();
                game->setCurrentState(game->getStateList().at(ENDGAME).get());
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                restoreCharactersStatus();
                switchTeam();
                game->setCurrentState(game->getStateList().at(ENDGAME).get());
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(ENDGAME).get());
            }
        } else {
            if (isTurnOver) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(PLAYERTURN).get());
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(PLAYERTURN).get());
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(PLAYERTURN).get());
            }
        }
    }

    void PlayerTurn::restoreCharactersStatus() {
        std::vector<Character> characters = game->getCurrentTeam().getCharacters();
        for (Character & character : characters)
        {
            if (character.getStatus() == played)
            {
                character.setStatus(playable);
            }
            else if (character.getStatus() == stunned)
            {
                character.setStatus(knockedDown);
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

    void PlayerTurn::simulateEndTurn() {
        endTurn = true;
    }

    void PlayerTurn::simulateTouchdown() {
        isTouchDown = true;
    }

    PlayerTurn::~PlayerTurn() {

    }

}
