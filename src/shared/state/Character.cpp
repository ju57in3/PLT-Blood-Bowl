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
        : name(std::move(name)), type(std::move(type)), movement(movement), strength(strength), agility(agility), armor(armor)
        {
            
            this->currentPosition = std::make_pair(-1,-1);
            this->hasBall = false;
            this->status = bench;
        }

    std::string Character::getCharacterName() const
    {
        return name;
    }

    void Character::setCharacterName(std::string nameValue)
    {
        this->name = nameValue;
    }

    std::string Character::getCharacterType() const
    {
        return type;
    }

    void Character::setCharacterType(std::string typeValue)
    {
        this->type = typeValue;
    }

    int Character::getMovementPoints() const
    {
        return movement;
    }

    void Character::setMovementPoints(int movementValue)
    {
        this->movement = movementValue;
    }

    int Character::getStrengthPoints() const
    {
        return strength;
    }

    void Character::setStrengthPoints(int strengthValue)
    {
        this->strength = strengthValue;
    }

    int Character::getAgilityPoints() const {
        return agility;
    }

    CharacterStatus Character::getStatus() const {
        return status;
    }

    void Character::setAgilityPoints(int agilityValue)
    {
        this->agility = agilityValue;
    }

    int Character::getArmorPoints() const
    {
        return armor;
    }

    void Character::setArmorPoints(int armor)
    {
        this->armor = armor;
    }

    std::pair<int,int> Character::getPosition() const {
        return currentPosition;
    }

    void Character::setPosition(std::pair<int,int> position) {
        this->currentPosition = position;
    }

    bool Character::getHasBall() const
    {
        return hasBall;
    }

    void Character::setHasBall(bool hasBallValue)
    {
        this->hasBall = hasBallValue;
    }

    void Character::setStatus(CharacterStatus newStatus) {
        this->status = newStatus;
    }

    std::ostream& operator<<(std::ostream& os, const Character& character) {
        os << "Position: (" << character.getPosition().first << ", " << character.getPosition().second << ")";
        os << " | Status: " << statusToString(character.getStatus());
        return os;
    }
}