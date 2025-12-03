#include "PickUpBall.h"
#include "utility/GameUtils.h"

#include <functional>

#include "state/BloodBowlGame.h"
#include "state/Character.h"

using namespace utility;

namespace engine {

PickUpBall::PickUpBall(std::shared_ptr<state::Character> character)
    : character(std::move(character)) {}

PickUpBall::~PickUpBall() = default;

CommandTypeId PickUpBall::getCommandTypeId() {
    return PickUpBallId;
}

void PickUpBall::execute(std::shared_ptr<state::BloodBowlGame> game) {
    if (!game || !character) return;

    auto ballPos = game->getBallPosition();
    auto charPos = character->getPosition();

    if (ballPos != charPos) return;

    bool success = GameUtils::agilityTest(character->getAgility(), 0);

    if (success) {
        character->setHasBall(true);
        game->setBallPosition(charPos);
        game->setBallIsHold(true);
        checkAndHandleTouchdown(game);
    } else {
        checkAndHandleTurnover(game);
    }

    if (character->getStatus() == state::CharacterStatus::playable) {
        character->setStatus(state::CharacterStatus::played);
    }
}

} // namespace engine
