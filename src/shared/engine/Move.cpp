// C++
#include "Move.h"
#include <algorithm>
#include <utility>
#include <random>
#include <chrono>
#include "utility/Constants.h"

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

                int agi = character->getAgility();
                std::uniform_int_distribution<int> d6(1, 6);
                int roll = d6(rng);

                if (roll <= agi) {
                    character->setHasBall(true);
                    game->setBallPosition(position);

                } else {
                    std::uniform_int_distribution<int> dirDist(-1, 1);
                    std::pair<int,int> bounce;
                    int attempts = 0;
                    do {
                        int dx = dirDist(rng);
                        int dy = dirDist(rng);
                        if (dx == 0 && dy == 0) continue;
                        bounce = { position.first + dx, position.second + dy };
                        attempts++;
                    } while ((bounce.first < 0 || bounce.first >= utility::Constants::BOARD_WIDTH ||
                              bounce.second < 0 || bounce.second >= utility::Constants::BOARD_HEIGHT) && attempts < 20);


                    game->setBallPosition(bounce);
                    checkAndHandleTurnover(game);
                }
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
