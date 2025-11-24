#include "Block.h"
#include <memory>

#include <state/PlayerTurn.h>
#include <iostream>
#include <random>
#include <utility>

static std::mt19937 rng(std::random_device{}());

namespace engine {

    // TODO : Fonction deja utliser dans Pass.cpp a generaliser !
    static std::pair<int,int> scatterOnce(std::pair<int,int> from) {
        std::uniform_int_distribution<int> d8(1,8);
        int dir = d8(rng);
        int dx = 0, dy = 0;
        switch(dir) {
            case 1: dy = 1; break;        // N
            case 2: dx = 1; dy = 1; break;// NE
            case 3: dx = 1; break;        // E
            case 4: dx = 1; dy = -1; break;// SE
            case 5: dy = -1; break;       // S
            case 6: dx = -1; dy = -1; break;// SW
            case 7: dx = -1; break;       // W
            case 8: dx = -1; dy = 1; break; // NW
            default: ;
        }
        int nx = from.first + dx;
        int ny = from.second + dy;
        return {nx, ny};
    }

    static void resolveInjury(const std::shared_ptr<state::Character>& targetCharacter)
    {
        std::uniform_int_distribution<int> d12(2,12);
        const int roll1 = d12(rng);
        if (roll1>= targetCharacter->getArmor())
        {
            const int injuryRoll = d12(rng);
            if (injuryRoll <= 7) {
                targetCharacter->setStatus(state::CharacterStatus::stunned);
            } else if (injuryRoll <= 9) {
                targetCharacter->setStatus(state::CharacterStatus::ko);
            } else {
                targetCharacter->setStatus(state::CharacterStatus::injured);
            }
        }
    }
    static int chooseDiceResult(std::vector<int> diceResults)
    {
        int answer = 0;
        std::cout << "Les choix de dés sont les suivants, entrez le score que vous souhaitez utiliser pour le bloc:" << std::endl;

        for (int diceResult : diceResults)
        {
            std::cout << diceResult << std::endl;
        }
        bool correctAnswer = false;
        while (!correctAnswer) {
            std::cin >> answer;

            for (int result : diceResults) {
                if (result == answer) {
                    correctAnswer = true;
                    break;
                }
            }
            if (!correctAnswer) {
                std::cout << "Choix invalide..." << std::endl;
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

        std::pair<int,int> oldPosition = defender->getPosition(); //Utile pour la poursuite de l'attaquant

        std::pair<int,int> newPosition1;
        std::pair<int,int> newPosition2;
        std::pair<int,int> newPosition3;

        // TODO : Verifier si la case est libre avant de la proposer
        if (Dx == 0) {
            newPosition1 = {defender_x - 1, defender_y + Dy};
            newPosition2 = {defender_x,     defender_y + Dy};
            newPosition3 = {defender_x + 1, defender_y + Dy};
        }else if (Dy == 0){
            newPosition1 = {defender_x + Dx, defender_y - 1};
            newPosition2 = {defender_x + Dx, defender_y};
            newPosition3 = {defender_x + Dx, defender_y + 1};
        }else{
            newPosition1 = {defender_x + Dx, defender_y};
            newPosition2 = {defender_x,     defender_y + Dy};
            newPosition3 = {defender_x + Dx, defender_y + Dy};
        }

        listOfPositions.push_back(newPosition1);
        listOfPositions.push_back(newPosition2);
        listOfPositions.push_back(newPosition3);


        bool correctAnswer = false;
        while (!correctAnswer) {
            std::cin >> answer;
            if (answer >= 1 && answer <= 3) {
                correctAnswer = true;
            }
            if (!correctAnswer) {
                std::cout << "Choix invalide..." << std::endl;
            }
        }
        correctAnswer = false; // Pour pouvoir l'utiliser pour la seconde requête

        std::pair<int,int> newPosition = listOfPositions[answer-1];
        defender->setPosition(newPosition);

        std::cout << "Vous avez choisi la position (" << listOfPositions[answer-1].first <<"," <<listOfPositions[answer-1].second << ") .\r\n" << std::endl;

        //Choix de poursuivre ou non
        int followDefender = -1;
        while (!correctAnswer)
        {
            std::cout << "Veuillez choisir si vous souhaitez poursuivre le défenseur avec votre attaquant en tapant 1 pour OUI ou 0 pour NON." << std::endl;
            std::cin >> followDefender;
            if (followDefender == 1)
            {
                attacker->setPosition(oldPosition);
                correctAnswer = true;
            }
            else if (followDefender == 0)
            {
                correctAnswer = true;
            }
        }
    }

    CommandTypeId Block::getCommandTypeId() {
        return BlockId;
    }

    static BlockResult rollBlockDice(const std::shared_ptr<state::Character>& attacker, const std::shared_ptr<state::Character>& defender)
    {
        std::vector<int> listOfDiceResult;
        std::uniform_int_distribution<int> d6(1,6);
        const int roll1 = d6(rng);
        const int roll2 = d6(rng);
        const int roll3 = d6(rng);

        listOfDiceResult.push_back(roll1); //Si l'attaquant et le défenseur ont le même nombre de points de force, il n'y a qu'un seul lancé de dé

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

        int diceResult;
        if (attacker->getStrength() >= defender->getStrength()) {
            diceResult = chooseDiceResult(listOfDiceResult); // Attaquant choisit
        } else {
            diceResult = chooseDiceResult(listOfDiceResult); // Défenseur choisit le pire
        }


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
        return Pushed; // Valeur par défaut.
        // TODO : Remplacer par une exception
    }

    Block::Block(std::shared_ptr<state::Character> attacker, std::shared_ptr<state::Character> defender)
    :attacker(std::move(attacker)), defender(std::move(defender)) {
        blockResult = rollBlockDice(this->attacker, this->defender);
    }

    Block::~Block() = default;


    void Block::execute(std::shared_ptr<state::BloodBowlGame> game)
    {
        if (blockResult == AttackerDown || blockResult == BothDown)
        {
            attacker->setStatus(state::CharacterStatus::knockedDown);
            resolveInjury(attacker);
            checkAndHandleTurnover(game);
        }
        if (blockResult == Pushed || blockResult == DefenderStumbles || blockResult == DefenderDown)
        {
            choosePushedPosition(attacker,defender);
        }
        if (blockResult == DefenderDown || blockResult == BothDown || blockResult == DefenderStumbles)
        {
            defender->setStatus(state::CharacterStatus::knockedDown);
            if (defender->getHasBall()) {
                defender->setHasBall(false);
                game->setBallPosition(scatterOnce(defender->getPosition()));
                game->setBallIsHold(false);
            }
            resolveInjury(defender);
        }
        // TODO : Implémenter la différence entre DefenderStumbles et DefenderDown (besoin de la capacité esquive).
    }
};