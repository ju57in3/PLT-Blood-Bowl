#include "PickUpBall.h"

#include <random>
#include <functional>

#include "state/BloodBowlGame.h"
#include "state/Character.h"
#include "state/PlayerTurn.h"
#include "utility/Constants.h"

static std::mt19937 rng(std::random_device{}());

namespace engine {

static std::pair<int,int> scatterOnce(std::pair<int,int> from) {
    std::uniform_int_distribution<int> d8(1,8);
    int dir = d8(rng);
    int dx = 0, dy = 0;
    switch(dir) {
        case 1: dy = 1; break;        // N
        case 2: dx = 1; dy = 1; break;// NE
        case 3: dx = 1; break;        // E
        case 4: dx = 1; dy = -1; break;// SE
        case 5: dy = -1; break;       // S
        case 6: dx = -1; dy = -1; break;// SW
        case 7: dx = -1; break;       // W
        case 8: dx = -1; dy = 1; break; // NW
        default: ;
    }
    int nx = from.first + dx;
    int ny = from.second + dy;

    // Clamp to board
    if (nx < 0) nx = 0;
    if (ny < 0) ny = 0;
    if (nx >= utility::Constants::BOARD_WIDTH) nx = utility::Constants::BOARD_WIDTH - 1;
    if (ny >= utility::Constants::BOARD_HEIGHT) ny = utility::Constants::BOARD_HEIGHT - 1;
    return {nx, ny};
}

static int agilityTarget(int ag) {
    if (ag <= 1) return 6;
    if (ag == 2) return 5;
    if (ag == 3) return 4;
    if (ag == 4) return 3;
    if (ag == 5) return 2;
    return 2;
}

static bool agilityTest(int agility, int modifiers) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> d6(1,6);
    int roll = d6(gen);
    if (roll == 1) return false;
    if (roll == 6) return true;
    int total = roll + modifiers;
    int need = agilityTarget(agility);
    return total >= need;
}

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

    // Only allow pickup if character is on the ball's square
    if (ballPos != charPos) return;

    // Perform agility test: no modifiers for now
    bool success = agilityTest(character->getAgility(), 0);

    if (success) {
        // Successful pickup: character holds the ball and game state updated
        character->setHasBall(true);
        game->setBallPosition(charPos);
        game->setBallIsHold(true);
        checkAndHandleTouchdown(game);
    } else {
        // Failed pickup: ball scatters to adjacent square and turnover
        auto scatterPos = scatterOnce(ballPos);
        game->setBallPosition(scatterPos);
        game->setBallIsHold(false);
        checkAndHandleTurnover(game);
    }

    if (character->getStatus() == state::CharacterStatus::playable) {
        character->setStatus(state::CharacterStatus::played);
    }
}

} // namespace engine
