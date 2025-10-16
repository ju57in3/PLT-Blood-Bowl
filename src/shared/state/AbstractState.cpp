#include "AbstractState.h"

namespace state {

AbstractState::AbstractState(BloodBowlGame* game) : game(game) {}

void AbstractState::update() {}

} // namespace state
