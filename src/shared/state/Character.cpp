#include "Character.h"

namespace state {
    bool Character::moveTo(Square* square) {
        currentSquare = square;
        return true;
    }

    CharacterStatus Character::getStatus(){
        return status;
    }

    Square* Character::getSquare(){
        return currentSquare;
    }

    void Character::setStatus(CharacterStatus newStatus) {
        this->status = status;
    }


}