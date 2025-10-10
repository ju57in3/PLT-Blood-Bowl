#include "Character.h"

namespace state {
    bool Character::moveTo(int position) {
        currentPosition = position;
        return true;
    }

    CharacterStatus Character::getStatus(){
        return status;
    }

    int Character::getPosition(){
        return currentPosition;
    }

    void Character::setStatus(CharacterStatus newStatus) {
        this->status = status;
    }


}