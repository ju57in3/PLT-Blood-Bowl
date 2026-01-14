#include "EndTurn.h"
#include "CommandTypeId.h"
#include "state/PlayerTurn.h"
#include "state/BloodBowlGame.h"
#include <iostream>
#include <memory>

namespace engine {

    EndTurn::EndTurn() {
        commandTypeId = EndTurnId;
    }

    EndTurn::~EndTurn() {}

    CommandTypeId EndTurn::getCommandTypeId() {
        return EndTurnId;
    }

    void EndTurn::execute(std::shared_ptr<state::BloodBowlGame> game) {
        if (!game) {
            std::cerr << "[EndTurn] No game provided!\n";
            return;
        }

        auto* currentState = game->getCurrentState();
        if (!currentState) {
            std::cerr << "[EndTurn] No current state!\n";
            return;
        }

        // Vérifier si on est bien dans l'état PlayerTurn
        if (auto* pt = dynamic_cast<state::PlayerTurn*>(currentState)) {
            std::cout << "[EndTurn] Ending turn for team " << game->getCurrentTeam()->getTeamId() << "\n";
            pt->setEndTurn(true);
            pt->update();
        } else {
            std::cerr << "[EndTurn] Cannot end turn, not in PlayerTurn state (current: "
                      << currentState->getName() << ")\n";
        }
    }

} // namespace engine

