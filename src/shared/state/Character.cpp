#include "Character.h"
#include <iostream>

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

    std::ostream& operator<<(std::ostream& os, Character& character) {
        os << "Position: (" << character.getPosition().first << ", " << character.getPosition().second << ")";
        os << " | Status: " << static_cast<int>(character.getStatus());
        return os;
    }

}