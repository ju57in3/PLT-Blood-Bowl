#include "Move.h"
#include <algorithm>
#include <utility>


namespace engine {
    Move::Move(std::shared_ptr<state::Character> character, std::pair<int, int> targetPosition)
        : position(std::move(targetPosition)), character(std::move(character)) {
        dodgeAttempts = 0;
        int current_xposition = character->getPosition().first;
        int current_yposition = character->getPosition().second;
        for (int i= -character->getMovement(); i<= character->getMovement();i++) {
            for (int j= -character->getMovement(); j<= character->getMovement(); j++) {
                range.push_back(std::make_pair(current_xposition + i,current_yposition + j)); // Add condition if case already taken
            }
        }
    }

    Move::~Move() {
    }

    CommandTypeId Move::getCommandTypeId() {
        return MoveId;
    }

    std::vector<std::pair<int, int> > Move::calculatePath(std::pair<int, int> dest) {
        return std::vector<std::pair<int, int> >();
    }

    bool Move::checkTackleZones() {
        bool isTackle = false;
        return isTackle;
    }

    void Move::execute(std::shared_ptr<state::BloodBowlGame> game) {
        if (std::find(range.begin(), range.end(), position) != range.end()) {
            character->setPosition(position);
        }
    }
}