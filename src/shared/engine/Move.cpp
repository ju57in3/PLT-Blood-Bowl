// C++
#include "Move.h"
#include <algorithm>
#include <utility>
#include <random>

#include "Engine.h"
#include "PickUpBall.h"
#include "state/PlayerTurn.h" // Added to check for turnover

static std::mt19937 rng(std::random_device{}());

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

    CommandTypeId Move::getCommandTypeId() {
        return MoveId;
    }

    void Move::execute(std::shared_ptr<state::BloodBowlGame> game) {

        if (std::find(range.begin(), range.end(), position) != range.end()) {
            // Save original position in case we need to revert (failed pickup)
            std::pair<int, int> originalPosition = character->getPosition();

            // Move the character to the new position
            character->setPosition(position);

            // If character has the ball, move it with them
            if (character->getHasBall()) {
                game->setBallPosition(position);
            }

            // Attempt to pick up ball if on the same square
            if (!character->getHasBall() && game->getBallPosition() == position) {
                PickUpBall pickUpBallCmd(character);
                pickUpBallCmd.execute(game);

                // Check if pickup failed and caused turnover
                if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                    if (pt->getTurnOver()) {
                        // Failed pickup - revert to position before this step
                        character->setPosition(originalPosition);
                        // InputHandler will detect turnover and stop further movement
                        return;
                    }
                }
            }

            // Check for touchdown
            checkAndHandleTouchdown(game);

            // Mark character as played (only after successful step)
            if (character->getStatus() == state::CharacterStatus::playable) {
                character->setStatus(state::CharacterStatus::played);
            }
        }
    }

    std::vector<std::pair<int, int> > Move::calculatePath(std::pair<int, int> dest) {
        return {};
    }

    bool Move::checkTackleZones() {
        bool isTackle = false;
        return isTackle;
    }
}
