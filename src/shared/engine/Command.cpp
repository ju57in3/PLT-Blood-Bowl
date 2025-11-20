#include "Command.h"

namespace engine {

    Command::Command() : commandTypeId(){}

    Command::~Command() {}

    CommandTypeId Command::getCommandTypeId() {}

    void Command::execute(std::shared_ptr<state::BloodBowlGame> game) {}


} // namespace engine