#include "Character.h"
#include <iostream>
#include <string>

namespace state {
    
    const char* statusToString(CharacterStatus status) {
        switch (status) {
            case knockedDown: return "knockedDown";
            case stunned:     return "stunned";
            case ko:          return "ko";
            case injured:     return "injured";
            case playable:    return "playable";
            case played:      return "played";
            case bench:       return "bench";
            default:          return "unknown";
        }
    }

    Character::Character(std::string name, std::string type, int movement, int strength, int agility, int armor)
        : name(name), type(type), movement(movement), strength(strength), agility(agility), armor(armor)
        {
            
            this->currentPosition = std::make_pair(-1,-1);
            this->hasBall = false;
            this->status = bench;
        }

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

    void Character::setPosition(std::pair<int,int> position) {
        this->currentPosition = position;
    }

    void Character::setStatus(CharacterStatus newStatus) {
        this->status = newStatus;
    }

    std::ostream& operator<<(std::ostream& os, Character& character) {
        os << "Position: (" << character.getPosition().first << ", " << character.getPosition().second << ")";
        os << " | Status: " << statusToString(character.getStatus());
        return os;
    }
}