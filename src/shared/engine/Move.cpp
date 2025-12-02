// C++
#include "Move.h"
#include <algorithm>
#include <utility>
#include <random>

#include "Engine.h"
#include "PickUpBall.h" // added to attempt pickup when moving onto ball

static std::mt19937 rng(std::random_device{}());

namespace engine {

    Move::Move(const std::shared_ptr<state::Character>& character, std::pair<int, int> targetPosition)
        : position(std::move(targetPosition)), character(character) {
        dodgeAttempts = 0;
        int current_xposition = character->getPosition().first;
        int current_yposition = character->getPosition().second;
        for (int i= -character->getMovement(); i<= character->getMovement();i++) {
            for (int j= -character->getMovement(); j<= character->getMovement(); j++) {
                range.emplace_back(current_xposition + i,current_yposition + j); // Add condition if case already taken
            }
        }
    }

    Move::~Move() = default;

    CommandTypeId Move::getCommandTypeId() {
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
            }
            checkAndHandleTouchdown(game);

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
