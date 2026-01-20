// C++
#include "Move.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <utility>
#include <random>

#include "Engine.h"
#include "PickUpBall.h"
#include "state/PlayerTurn.h" // Added to check for turnover

namespace engine {

    Move::Move(const std::shared_ptr<state::Character>& character, std::pair<int, int> targetPosition)
        : position(std::move(targetPosition)), character(character) {
        dodgeAttempts = 0;
        int current_xposition = character->getPosition().first;
        int current_yposition = character->getPosition().second;
        for (int i= -character->getMovement(); i<= character->getMovement();i++) {
            for (int j= -character->getMovement(); j<= character->getMovement(); j++) {
                range.emplace_back(current_xposition + i,current_yposition + j);
            }
        }
    }

    Move::~Move() = default;

    CommandTypeId Move::getCommandTypeId() const {
        return MoveId;
    }

    void Move::execute(std::shared_ptr<state::BloodBowlGame> game) {
        if (std::find(range.begin(), range.end(), position) != range.end()) {
            character->setPosition(position);

            if (character->getHasBall()) {
                game->setBallPosition(position);
            }

            if (!character->getHasBall() && game->getBallPosition() == position) {
                PickUpBall pickUpBallCmd(character);
                pickUpBallCmd.execute(game);

                if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                    if (pt->getTurnOver()) {
                        bool outOfBounds = false;
                        bool turnover = false;

                        // Déterminer l'équipe du personnage
                        state::Team* characterTeam = nullptr;
                        for (auto& c : game->getTeamA().getCharacters()) {
                            if (c.get() == character.get()) {
                                characterTeam = &game->getTeamA();
                                break;
                            }
                        }
                        if (!characterTeam) {
                            for (auto& c : game->getTeamB().getCharacters()) {
                                if (c.get() == character.get()) {
                                    characterTeam = &game->getTeamB();
                                    break;
                                }
                            }
                        }

                        utility::GameUtils::handleBallBounce(game, position, outOfBounds, turnover, characterTeam);
                        return;
                    }
                }
            }

            checkAndHandleTouchdown(game);

            if (character->getStatus() == state::CharacterStatus::playable) {
                character->setStatus(state::CharacterStatus::played);
            }
        }
    }


    // TODO : Unused ?
    /*
    std::vector<std::pair<int, int> > Move::calculatePath(std::pair<int, int> dest) {
        return {};
    }
    */

    bool Move::checkTackleZones() {
        bool isTackle = false;
        return isTackle;
    }
}
