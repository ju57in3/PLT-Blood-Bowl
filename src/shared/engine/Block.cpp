#include "Block.h"
#include <memory>

#include <state/PlayerTurn.h>
#include <iostream>

namespace engine {
    Block::Block(std::shared_ptr<state::Character> attacker, std::shared_ptr<state::Character> defender):attacker(attacker), defender(defender){}

    static int chooseDiceResult(std::vector<int> diceResults)
    {
        int answer = 0;
        std::cout << "Les choix de dés sont les suivants, entrez le score que vous souhaitez utiliser pour le bloc:" << std::endl;

        for (int i = 0; i < diceResults.size(); i++)
        {
            std::cout << diceResults[i] << std::endl;
        }
        bool correctAnswer = false;
        while (!correctAnswer)
        {
            std::cin >> answer;

            for (int i = 0; i < diceResults.size(); i++)
            {
                if (diceResults[i] == answer)
                {
                    correctAnswer = true;
                }
                if (diceResults[i] != answer && i == diceResults.size() - 1)
                {
                    std::cout << "Choix invalide. Veuillez entrer un score valide." << std::endl;
                }
            }
        }
        std::cout << "Vous avez choisi le score: " << answer << std::endl;
        return answer;
    }

    static void choosePushedPosition(std::shared_ptr<state::Character> attacker, std::shared_ptr<state::Character> defender)
    {
        int answer;
        std::vector<std::pair<int,int>> listOfPositions;
        int attacker_x = attacker->getPosition().first;
        int attacker_y = attacker->getPosition().second;
        int defender_x = defender->getPosition().first;
        int defender_y = defender->getPosition().second;

        int Dx = defender_x - attacker_x;
        int Dy = defender_y - attacker_y;


        std::pair<int,int> newPosition1;
        std::pair<int,int> newPosition2;
        std::pair<int,int> newPosition3;
        if (Dx == 0)
        {
            newPosition1.first= defender_x - 1;
            newPosition1.second = defender_y + Dy;

            newPosition1.first= defender_x;
            newPosition1.second = defender_y + Dy;

            newPosition1.first= defender_x + 1;
            newPosition1.second = defender_y + Dy;
        }
        else if (Dy == 0)
        {
            newPosition1.first= defender_x + Dx;
            newPosition1.second = defender_y + 1;

            newPosition2.first= defender_x + Dx;
            newPosition2.second = defender_y;

            newPosition3.first= defender_x + Dx;
            newPosition3.second = defender_y -1;
        }
        else
        {
            newPosition1.first= defender_x ;
            newPosition1.second = defender_y + Dy;

            newPosition2.first= defender_x + Dx;
            newPosition2.second = defender_y + Dy;

            newPosition3.first= defender_x + Dx;
            newPosition3.second = defender_y;
        }

        std::pair<int,int> newPosition = listOfPositions[answer-1];
        defender->setPosition(newPosition);

    }

    CommandTypeId Block::getCommandTypeId() {
        return BlockId;
    }

    static BlockResult rollBlockDice(std::shared_ptr<state::Character> attacker, std::shared_ptr<state::Character> defender)
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
            return AttackerDown;
        }
        if (diceResult == 2)
        {
            return BothDown;
        }
        if (diceResult == 3 || diceResult == 4)
        {
            return Pushed;
        }
        if (diceResult == 5)
        {
            return DefenderStumbles;
        }
        if (diceResult == 6)
        {
            return DefenderDown;
        }
    }

    void resolveInjury(std::shared_ptr<state::Character> targetCharacter)
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
        }
    }

    void Block::execute(std::shared_ptr<state::BloodBowlGame> game)
    {
        blockResult=rollBlockDice(attacker,defender);

        if (blockResult == AttackerDown || blockResult == BothDown)
        {
            resolveInjury(attacker);
            //TurnOver !!  Utilisation de la fonction setTurnOver de PlayerTurn (Voir compléter fonction update() de PlayerTurn! -> if (turnOver))
        }
        if (blockResult == Pushed || blockResult == DefenderStumbles)
        {
            // choosePushedPosition(attacker->getPosition(),defender->getPosition());
            choosePushedPosition(attacker,defender);

        }
        if (blockResult == DefenderDown || blockResult == BothDown) //Si pas d'esquive, la dernière boucle s'inclue ici.
        {
            resolveInjury(defender);
        }
        if (blockResult == DefenderStumbles)
        {
            //Jet d'esquive ?
            resolveInjury(defender);
        }
    }

};