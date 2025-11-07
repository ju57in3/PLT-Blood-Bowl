#include <iostream>
#include "PlayerTurn.h"
#include "BloodBowlGame.h"
#include "EndGame.h"
#include "HalfTime.h"
#include "Setup.h"
#include "Team.h"
#include "utility/Constants.h"

using namespace utility;

namespace state {
    PlayerTurn::PlayerTurn(BloodBowlGame* game) : AbstractState(game) {
        isBlitzAvailable = true;
        isTurnOver = false;
        isTouchDown = false;
        endTurn = false;
        teamId = game->getCurrentTeam()->getTeamId();

    }

    void PlayerTurn::update() {
        bool scored = isTouchDown;
        bool turned = isTurnOver || endTurn;

        if (!scored && !turned) {
            isTurnOver = false;
            isTouchDown = false;
            endTurn = false;
            return;
        }

        if (scored) {
            game->getCurrentTeam()->setScore(game->getCurrentTeam()->getScore() + 1);
        }

        restoreCharactersStatus();

        switchTeam();

        game->setTurnCounter(game->getTurnCounter() + 1);

        int tc = game->getTurnCounter();
        if (scored) {
            // After a touchdown we go to setup for the next kickoff/setup
            game->setCurrentState(game->getStateList().at(SETUP).get());
        } else if (tc == Constants::HALF_TIME_TURN) {
            game->setCurrentState(game->getStateList().at(HALFTIME).get());
        } else if (tc == Constants::END_GAME_TURN) {
            game->setCurrentState(game->getStateList().at(ENDGAME).get());
        } else {
            game->setCurrentState(game->getStateList().at(PLAYERTURN).get());
        }

        isTurnOver = false;
        isTouchDown = false;
        endTurn = false;

    }

    void PlayerTurn::restoreCharactersStatus() {
        for (auto& pptr : game->getCurrentTeam()->getCharacters()) {
            if (pptr != nullptr) {
                Character& character = *pptr;
                if (character.getStatus() == played) {
                    character.setStatus(playable);
                } else if (character.getStatus() == stunned) {
                    character.setStatus(knockedDown);
                }
            }
        }
    }


    void PlayerTurn::switchTeam() {
        if (game->getCurrentTeam()->getTeamId() == game->getTeamA().getTeamId()) {
            game->setCurrentTeam(&game->getTeamB());
        } else {
            game->setCurrentTeam(&game->getTeamA());
        }
    }

    bool PlayerTurn::getTurnOver () const
    {
        return isTurnOver;
    }
    void PlayerTurn::setTurnOver (bool turnOver)
    {
        isTurnOver = turnOver;
    }
    bool PlayerTurn::getTouchDown () const
    {
        return isTouchDown;
    }
    void PlayerTurn::setTouchDown (bool touchDown)
    {
        isTouchDown = touchDown;
    }
    bool PlayerTurn::getEndTurn () const
    {
        return endTurn;
    }
    void PlayerTurn::setEndTurn (bool turnEnded)
    {
        endTurn = turnEnded;
    }

    PlayerTurn::~PlayerTurn() {

    }

    std::string PlayerTurn::getName() const {
        return "PlayerTurn";
    }
}
