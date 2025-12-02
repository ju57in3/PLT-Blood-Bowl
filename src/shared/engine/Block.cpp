#include "Block.h"
#include "utility/GameUtils.h"
#include <memory>

#include <state/PlayerTurn.h>
#include <random>
#include <utility>
#include <algorithm>

using namespace utility;

namespace engine {

    static void resolveInjury(const std::shared_ptr<state::Character>& targetCharacter)
    {
        std::uniform_int_distribution<int> d12(2,12);
        const int roll1 = d12(GameUtils::getRNG());
        if (roll1 >= targetCharacter->getArmor())
        {
            const int injuryRoll = d12(GameUtils::getRNG());
            if (injuryRoll <= 7) {
                targetCharacter->setStatus(state::CharacterStatus::stunned);
            } else if (injuryRoll <= 9) {
                targetCharacter->setStatus(state::CharacterStatus::ko);
            } else {
                targetCharacter->setStatus(state::CharacterStatus::injured);
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
        if (diceOptions.empty()) {
            generateDiceOptions();
        }

        if (diceOptions.empty()) {
            chosenDiceValue = 3;
        } else if (chosenIndex >= 1 && chosenIndex <= static_cast<int>(diceOptions.size())) {
            chosenDiceValue = diceOptions[chosenIndex-1];
        } else {
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
            return;
        }
        if (diceResult == 5)
        {
            blockResult = DefenderStumbles;
            return;
        }
        if (diceResult == 6)
        {
            blockResult = DefenderDown;
            return;
        }
        blockResult = Pushed;
    }

    void Block::choosePushedPositionNonInteractive() {
        if (!attacker || !defender) return;
        int attacker_x = attacker->getPosition().first;
        int attacker_y = attacker->getPosition().second;
        int defender_x = defender->getPosition().first;
        int defender_y = defender->getPosition().second;

        int Dx = defender_x - attacker_x;
        int Dy = defender_y - attacker_y;

        std::pair<int,int> newPosition = defender->getPosition();
        if (Dx == 0) {
            newPosition.first = defender_x;
            newPosition.second = defender_y + (Dy>0?1:-1);
        } else if (Dy == 0) {
            newPosition.first = defender_x + (Dx>0?1:-1);
            newPosition.second = defender_y;
        } else {
            newPosition.first = defender_x + (Dx>0?1:-1);
            newPosition.second = defender_y + (Dy>0?1:-1);
        }
        defender->setPosition(newPosition);
    }

    void Block::execute(std::shared_ptr<state::BloodBowlGame> game)
    {
        if (!choiceApplied) {
            applyDiceChoice(-1);
        }

        if (blockResult == AttackerDown || blockResult == BothDown)
        {
            if (attacker) {
                attacker->setStatus(state::CharacterStatus::knockedDown);
                if (attacker->getHasBall()) {
                    attacker->setHasBall(false);
                    bool outOfBounds = false;
                    GameUtils::handleBallBounce(game, attacker->getPosition(), outOfBounds);
                    if (outOfBounds) {
                        checkAndHandleTurnover(game);
                    }
                }
                resolveInjury(attacker);
                checkAndHandleTurnover(game);
            }
        }
        if (blockResult == Pushed || blockResult == DefenderStumbles || blockResult == DefenderDown)
        {
            choosePushedPositionNonInteractive();
        }
        if (blockResult == DefenderDown || blockResult == BothDown || blockResult == DefenderStumbles)
        {
            if (defender) {
                defender->setStatus(state::CharacterStatus::knockedDown);
                if (defender->getHasBall()) {
                    defender->setHasBall(false);
                    bool outOfBounds = false;
                    GameUtils::handleBallBounce(game, defender->getPosition(), outOfBounds);
                    if (outOfBounds) {
                        checkAndHandleTurnover(game);
                    }
                }
                resolveInjury(defender);
            }
        }
        // TODO : Implémenter la différence entre DefenderStumbles et DefenderDown (besoin de la capacité esquive).

        // If attacker survived and was playable, mark them as played
        if (attacker && attacker->getStatus() == state::CharacterStatus::playable) {
            attacker->setStatus(state::CharacterStatus::played);
        }
    }
};