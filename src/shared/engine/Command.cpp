#include "Command.h"

#include <ostream>

#include "state/PlayerTurn.h"
#include "utility/Constants.h"

namespace engine {

    Command::Command() : commandTypeId(){}

    Command::~Command() {}

    CommandTypeId Command::getCommandTypeId() {
        return commandTypeId;
    }

    void Command::execute(std::shared_ptr<state::BloodBowlGame> game) {}

    void Command::checkAndHandleTouchdown(const std::shared_ptr<state::BloodBowlGame>& game) {
        if (!game) return;

        int tdColumnTeamA = utility::Constants::BOARD_WIDTH - 1;
        int tdColumnTeamB = 0;

        auto ballPos = game->getBallPosition();

        if (ballPos.first == tdColumnTeamA) {
            game->getTeamA().setScore(game->getTeamA().getScore() + 1);

            if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                pt->setTouchDown(true);
            }
        } else if (ballPos.first == tdColumnTeamB) {
            game->getTeamB().setScore(game->getTeamB().getScore() + 1);

            if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                pt->setTouchDown(true);
            }
        }
    }

    void Command::checkAndHandleTurnover(const std::shared_ptr<state::BloodBowlGame>& game) {
        if (!game) return;

        if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
            pt->setTurnOver(true);
        }
    }

} // namespace engine