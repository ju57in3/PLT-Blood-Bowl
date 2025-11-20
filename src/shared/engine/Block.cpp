#include "Block.h"
#include <memory>

#include <state/PlayerTurn.h>
#include <iostream>

namespace engine {
    Block::Block(std::shared_ptr<state::Character> attacker, std::shared_ptr<state::Character> defender):attacker(attacker), defender(defender){}

    Block::~Block() = default;

    CommandTypeId Block::getCommandTypeId() {
        return BlockId;
    }

    int chooseDiceResult(std::vector<int>)
    {
        return 1; // return 0; mettre à jour
    }
    void Block::rollBlockDice()
    {
        bool answered = false;
        std::vector<int> listOfDiceResult;
        int range = 6; //Range lancé de Dé pour le Block
        int roll1 = rand() % range + 1;
        int roll2 = rand() % range + 1;
        int roll3 = rand() % range + 1;

        listOfDiceResult.push_back(roll1); //Si l'attaquant et le défenseur ont le même nombre de points de force, il n'y a qu'un seul lancé de dé

        if (attacker->getStrength() > defender->getStrength())
        {
            listOfDiceResult.push_back(roll2);
            if (attacker->getStrength()>= 2*defender->getStrength())
            {
                listOfDiceResult.push_back(roll3);
            }
        }

        else if (attacker->getStrength() < defender->getStrength())
        {
            listOfDiceResult.push_back(roll2);
            if (attacker->getStrength()>= 2*defender->getStrength())
            {
                listOfDiceResult.push_back(roll3);
            }
        }

        int diceResult = chooseDiceResult(listOfDiceResult);

        if (diceResult == 1)
        {
            blockResult = AttackerDown;
        }
        if (diceResult == 2)
        {
            blockResult = BothDown;
        }
        if (diceResult == 3 || diceResult == 4)
        {
            blockResult = Pushed;
        }
        if (diceResult == 5)
        {
            blockResult = DefenderStumbles;
        }
        if (diceResult == 6)
        {
            blockResult = DefenderDown;
        }
    }

    void Block::resolveInjury(std::shared_ptr<state::Character> targetCharacter)
    {
        int roll1 = rand() % 11 + 1;
        if (roll1>= targetCharacter->getArmor())
        {
            int roll2 = rand() % 6 + 1;
            int roll3 = rand() % 6 + 1;
            if (roll2 + roll3 <= 7)
            {
                targetCharacter->setStatus(state::CharacterStatus::knockedDown);
            }
            else if (roll2 + roll3 <= 9)
            {
                targetCharacter->setStatus(state::CharacterStatus::injured);
            }
            else if (roll2 + roll3 <= 12)
            {
                targetCharacter->setStatus(state::CharacterStatus::ko);
            }

            // manque le cas de blessure
        }
    }

    void Block::execute(std::shared_ptr<state::BloodBowlGame> game)
    {
        rollBlockDice();
        if (blockResult == AttackerDown || blockResult == BothDown)
        {
            resolveInjury(attacker);
            //TurnOver !!  Utilisation de la fonction setTurnOver de PlayerTurn (Voir compléter fonction update() de PlayerTurn! -> if (turnOver))
        }
        if (blockResult == Pushed || blockResult == DefenderStumbles)
        {
            std::pair<int,int> target1;
            std::pair<int,int> target2;
            std::pair<int,int> target3;


            /*
            std::pair<int,int> newTarget;
            std::pair<int,int> actualTarget;

            actualTarget.first = defender->getPosition().first;
            actualTarget.second = defender->getPosition().second;

            newTarget.first = defender->getPosition().first + (defender->getPosition().first - attacker->getPosition().first);
            newTarget.second = defender->getPosition().second;

            defender->setPosition(newTarget);
            attacker->setPosition(actualTarget); //Pour l'instant, on considère que l'attaquant prend obligatoirement la place du défenseur. Il faudra coder une requête auprès de l'utilisateur.
            */
        }
        if (blockResult == DefenderDown || blockResult == BothDown)
        {
            resolveInjury(defender);
        }
        if (blockResult == DefenderStumbles)
        {
            //Gestion Esquive du plaquage. A faire plus tard. En attendant, on considère qu'il est plaqué à chaque fois
            resolveInjury(defender);
        }
    }

};