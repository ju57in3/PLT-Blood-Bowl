#include <iostream>
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
                game->setCurrentState(game->getStateList().at(HALFTIME));
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(HALFTIME));
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(HALFTIME));
            }
        } else if (game->getTurnCounter() == 32) {
            if (isTurnOver) {
                restoreCharactersStatus();
                switchTeam();
                game->setCurrentState(game->getStateList().at(ENDGAME));
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                restoreCharactersStatus();
                switchTeam();
                game->setCurrentState(game->getStateList().at(ENDGAME));
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(ENDGAME));
            }
        } else {
            if (isTurnOver) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(PLAYERTURN));
            }
            if (isTouchDown) {
                game->getCurrentTeam().setScore(game->getCurrentTeam().getScore() + 1);
                std::cout << "Le score de l'équipe " << game->getCurrentTeam().getTeamId() << " devrait etre de " << game->getCurrentTeam().getScore() + 1 << std::endl;
                endTurn = true;
            }
            if (endTurn) {
                restoreCharactersStatus();
                switchTeam();
                game->setTurnCounter(game->getTurnCounter() + 1);
                game->setCurrentState(game->getStateList().at(PLAYERTURN));

            }
        }
        isTurnOver = false;
        isTouchDown = false;
        endTurn = false;

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

    bool PlayerTurn::getTurnOver ()
    {
        return isTurnOver;
    }
    void PlayerTurn::setTurnOver (bool turnOver)
    {
        isTurnOver = turnOver;
    }
    bool PlayerTurn::getTouchDown (bool touchDown)
    {
        return isTouchDown;
    }
    void PlayerTurn::setTouchDown (bool touchDown)
    {
        isTouchDown = touchDown;
    }
    bool PlayerTurn::getEndTurn ()
    {
        return endTurn;
    }
    void PlayerTurn::setEndTurn (bool turnEnded)
    {
        endTurn = turnEnded;
    }
}
