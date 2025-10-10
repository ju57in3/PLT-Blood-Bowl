#include "Character.h"

namespace state {
    bool Character::moveTo(std::pair<int,int> position) {
        currentPosition = position;
        return true;
    }

    CharacterStatus Character::getStatus(){
        return status;
    }

    std::pair<int,int> Character::getPosition(){
        return currentPosition;
    }

    void Character::setStatus(CharacterStatus newStatus) {
        this->status = newStatus;
    }


}