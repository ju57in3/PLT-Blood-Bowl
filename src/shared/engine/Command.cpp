#include "Command.h"
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

        int tdColumnTeamA = utility::Constants::BOARD_WIDTH;
        int tdColumnTeamB = 0;

        auto ballPos = game->getBallPosition();

        if (ballPos.first == tdColumnTeamA) {
            game->getTeamA().setScore(game->getTeamA().getScore() + 1);

            // Signal touchdown to state machine
            if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                pt->setTouchDown(true);
                pt->update();
            }
        } else if (ballPos.first == tdColumnTeamB) {
            game->getTeamB().setScore(game->getTeamB().getScore() + 1);

            if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                pt->setTouchDown(true);
                pt->update();
            }
        }
    }

    void Command::checkAndHandleTurnover(const std::shared_ptr<state::BloodBowlGame>& game) {
        if (!game) return;

        // Signal turnover to state machine: mark turn over and request update to trigger state transition
        if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
            pt->setTurnOver(true);
            pt->update();
        }
    }

} // namespace engine