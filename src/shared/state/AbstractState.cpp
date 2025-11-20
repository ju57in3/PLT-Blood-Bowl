#include "AbstractState.h"

namespace state {

    AbstractState::AbstractState(BloodBowlGame* game) : game(game) {}

    void AbstractState::update() {}

    std::string AbstractState::getName() const {
        return "AbstractState";
    }

    AbstractState::~AbstractState() {}
} // namespace state
