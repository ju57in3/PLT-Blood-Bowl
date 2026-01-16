#include "Block.h"
#include "utility/GameUtils.h"
#include <memory>

#include <state/PlayerTurn.h>
#include <random>
#include <utility>
#include <algorithm>
#include "Move.h"
#include <iostream>

using namespace utility;

namespace engine {

    static void resolveInjury(const std::shared_ptr<state::Character>& targetCharacter)
    {
        std::uniform_int_distribution<int> d6(1,6);
        const int roll1 = d6(GameUtils::getRNG()) + d6(GameUtils::getRNG());
        if (roll1>= targetCharacter->getArmor())
        {
            const int injuryRoll = d6(GameUtils::getRNG()) + d6(GameUtils::getRNG());
            if (injuryRoll <= 7) {
                targetCharacter->setStatus(state::CharacterStatus::stunned);
                std::cout << "Stunned!" << std::endl;
            } else if (injuryRoll <= 9) {
                targetCharacter->setStatus(state::CharacterStatus::ko);
                targetCharacter->setPosition(std::make_pair(-1, -1));
                std::cout << "KO!" << std::endl;
            } else {
                targetCharacter->setStatus(state::CharacterStatus::injured);
                targetCharacter->setPosition(std::make_pair(-1, -1));
                std::cout << "Injured!" << std::endl;
            }
        }
    }

    CommandTypeId Block::getCommandTypeId() {
        return BlockId;
    }

    static std::vector<int> rollBlockDiceOptions(const std::shared_ptr<state::Character>& attacker, const std::shared_ptr<state::Character>& defender) {
        std::vector<int> listOfDiceResult;
        std::uniform_int_distribution<int> d6(1,6);
        const int roll1 = d6(GameUtils::getRNG());
        const int roll2 = d6(GameUtils::getRNG());
        const int roll3 = d6(GameUtils::getRNG());

        listOfDiceResult.push_back(roll1); // first always

        if (attacker->getStrength() > defender->getStrength()) {
            listOfDiceResult.push_back(roll2);
            if (attacker->getStrength() >= 2*defender->getStrength())
            {
                listOfDiceResult.push_back(roll3);
            }
        }

        else if (attacker->getStrength() < defender->getStrength())
        {
            listOfDiceResult.push_back(roll2);
            if (2*attacker->getStrength() <= defender->getStrength())
            {
                listOfDiceResult.push_back(roll3);
            }
        }
        return listOfDiceResult;
    }

    Block::Block(std::shared_ptr<state::Character> attacker, std::shared_ptr<state::Character> defender)
    :attacker(std::move(attacker)), defender(std::move(defender)) {
        if (this->attacker && this->defender) {
            diceOptions = rollBlockDiceOptions(this->attacker, this->defender);
            choiceApplied = false;
            chosenDiceValue = 0;
        } else {
            diceOptions.clear();
            choiceApplied = true;
        }
    }

    Block::~Block() = default;

    std::vector<int> Block::getDiceOptions() const {
        return diceOptions;
    }

    void Block::generateDiceOptions() {
        if (attacker && defender) {
            diceOptions = rollBlockDiceOptions(attacker, defender);
        } else {
            diceOptions.clear();
        }
    }

    void Block::applyDiceChoice(int chosenIndex) {
         if (chosenIndex >= 1 && chosenIndex <= static_cast<int>(diceOptions.size())) {
            chosenDiceValue = diceOptions[chosenIndex-1];
        } else {  //Automatic selection of the dice
            if (attacker && defender && attacker->getStrength() >= defender->getStrength()) {
                chosenDiceValue = *std::max_element(diceOptions.begin(), diceOptions.end());
            } else {
                chosenDiceValue = *std::min_element(diceOptions.begin(), diceOptions.end());
            }
        }
        choiceApplied = true;
        computeBlockResultFromChosenValue();
    }

    void Block::computeBlockResultFromChosenValue() {
        int diceResult = chosenDiceValue;
        if (diceResult == 1)
        {
            blockResult = AttackerDown;
            return;
        }
        if (diceResult == 2)
        {
            blockResult = BothDown;
            return;
        }
        if (diceResult == 3 || diceResult == 4)
        {
            blockResult = Pushed;
            enemyPushed = true;
            return;
        }
        if (diceResult == 5)
        {
            blockResult = DefenderStumbles;
            enemyPushed = true;
            return;
        }
        if (diceResult == 6)
        {
            blockResult = DefenderDown;
            enemyPushed = true;
            return;
        }
        blockResult = Pushed;
    }

    std::vector<std::pair<int,int>> Block::getPushedPositionOptions() {
        std::vector<std::pair<int,int>> positionOptions;

        int attacker_x = attacker->getPosition().first;
        int attacker_y = attacker->getPosition().second;
        int defender_x = defender->getPosition().first;
        int defender_y = defender->getPosition().second;

        int Dx = defender_x - attacker_x;
        int Dy = defender_y - attacker_y;

        std::pair<int,int> newPosition1;
        std::pair<int,int> newPosition2;
        std::pair<int,int> newPosition3;

        if (Dx == 0) {
            newPosition1.first = defender_x - 1;
            newPosition1.second = defender_y + Dy;

            newPosition2.first = defender_x;
            newPosition2.second = defender_y + Dy;

            newPosition3.first = defender_x + 1;
            newPosition3.second = defender_y + Dy;
        } else if (Dy == 0) {
            newPosition1.first = defender_x + Dx;
            newPosition1.second = defender_y + 1;

            newPosition2.first = defender_x + Dx;
            newPosition2.second = defender_y;

            newPosition3.first = defender_x + Dx;
            newPosition3.second = defender_y - 1;
        } else {
            newPosition1.first = defender_x;
            newPosition1.second = defender_y + Dy;

            newPosition2.first = defender_x + Dx;
            newPosition2.second = defender_y + Dy;

            newPosition3.first = defender_x + Dx;
            newPosition3.second = defender_y;
        }

        positionOptions.push_back(newPosition1);
        positionOptions.push_back(newPosition2);
        positionOptions.push_back(newPosition3);

        return positionOptions;
    }

    void Block::applyPushedPositionChoice(std::pair<int, int> targetPosition)
    {
        holdDefenderPosition = defender->getPosition();
        defender->setPosition(targetPosition);
        //TODO: Mouvement du ballon avec le défenseur
    }

    std::pair<int,int> Block::getHoldDefenderPosition() const
    {
        return holdDefenderPosition;
    }

    void Block::execute(std::shared_ptr<state::BloodBowlGame> game)
    {
        if (!choiceApplied) {
            applyDiceChoice(-1);
        }

        if (blockResult == AttackerDown)
        {
            if (attacker) {
                attacker->setStatus(state::CharacterStatus::knockedDown);

                int xA = attacker->getPosition().first;
                int yA = attacker->getPosition().second;
                int xD = defender->getPosition().first;
                int yD = defender->getPosition().second;
                std::pair<int,int> newPosition1;
                newPosition1.first = xA + (xA - xD);
                newPosition1.second = yA + (yA - yD);
                attacker->setPosition(newPosition1);

                resolveInjury(attacker);
                checkAndHandleTurnover(game);
            }
        }

        if (blockResult == BothDown)
        {
            if (attacker) {
                attacker->setStatus(state::CharacterStatus::knockedDown);
                if (attacker->getHasBall()) {
                    attacker->setHasBall(false);
                    bool outOfBounds = false;
                    bool ballTurnover = false;

                    // Déterminer l'équipe de l'attaquant
                    state::Team* attackerTeam = nullptr;
                    for (auto& c : game->getTeamA().getCharacters()) {
                        if (c.get() == attacker.get()) {
                            attackerTeam = &game->getTeamA();
                            break;
                        }
                    }
                    if (!attackerTeam) {
                        for (auto& c : game->getTeamB().getCharacters()) {
                            if (c.get() == attacker.get()) {
                                attackerTeam = &game->getTeamB();
                                break;
                            }
                        }
                    }

                    GameUtils::handleBallBounce(game, attacker->getPosition(), outOfBounds, ballTurnover, attackerTeam);
                }
                resolveInjury(attacker);
                checkAndHandleTurnover(game);
            }
        }

        if (blockResult == Pushed || blockResult == DefenderStumbles || blockResult == DefenderDown)
        {
            //TODO: Gestion esquive -> différentiation des cas de poussée
        }

        if (blockResult == DefenderDown || blockResult == BothDown)
        {
            if (defender) {
                defender->setStatus(state::CharacterStatus::knockedDown);
                if (defender->getHasBall()) {
                    defender->setHasBall(false);
                    bool outOfBounds = false;
                    bool ballTurnover = false;
                    GameUtils::handleBallBounce(game, defender->getPosition(), outOfBounds, ballTurnover);
                }
                resolveInjury(defender);
            }
        }

        // TODO: Implement Dodge test - if defender has Dodge skill and test succeeds, they stay standing

        if (attacker && attacker->getStatus() == state::CharacterStatus::playable) {
            attacker->setStatus(state::CharacterStatus::played);
        }
    }//

    bool Block::getEnemyPushed() const{
        return enemyPushed;
    }
};