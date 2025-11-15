#include "Block.h"
#include <memory>

#include <state/PlayerTurn.h>

namespace engine {
    Block::Block(std::shared_ptr<state::Character> attacker, std::shared_ptr<state::Character> defender):attacker(attacker), defender(defender){}

    void Block::rollBlockDice()
    {
        int diceResult;
        int range = 6; //Range lancé de Dé pour le Block
        int roll1 = rollDice(6);
        diceResult = roll1; //Si l'attaquant et le défenseur ont le même nombre de points de force, il n'y a qu'un seul lancé de dé

        if (attacker->getStrength() > defender->getStrength())
        {
            int roll2 = rollDice(6);
            diceResult = std::max(roll1,roll2);  // Le choix arrangeant le plus l'attaquant sont les résultats élevés
            // pas forcément : l(attaquant doit choisir dois choisir le résultat.
            if (attacker->getStrength()>= 2*defender->getStrength())
            {
                int roll3 = rollDice(6);
                diceResult = std::max(roll3,diceResult);
            }
        }

        else if (attacker->getStrength() < defender->getStrength())
        {
            int roll2 = rollDice(6);
            diceResult = std::min(roll1,roll2); // Le choix arrangeant le plus le défenseur sont les résultats faibles
            // meme chose... le defenseur doit choisir ce q'il veut.
            if (attacker->getStrength()>= 2*defender->getStrength())
            {
                int roll3 = rollDice(6);
                diceResult = std::min(roll3,diceResult);
            }
        }

        // bares ne va pas etre content mdrr
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
        int roll1 = rollDice(6);
        int roll2 = rollDice(6);
        // why not : rollDice(12); ??
        if (roll1 + roll2 >= targetCharacter->getArmor())
        {
            roll1 = rollDice(6);
            roll2 = rollDice(6);
            if (roll1 + roll2 <= 7)
            {
                targetCharacter->setStatus(state::CharacterStatus::knockedDown);
            }
            else if (roll1 + roll2 <= 9)
            {
                targetCharacter->setStatus(state::CharacterStatus::injured);
            }
            else if (roll1 + roll2 <= 12)
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
            //Il faudra voir si on autorise la poussée du joueur defenseur sur d'autres cases que celle derrière lui.
            //Peut-on tacler un joueur dans le sens adverses du jeu? Verticalement? // OUI
            std::pair<int,int> newTarget;
            std::pair<int,int> actualTarget;

            actualTarget.first = defender->getPosition().first;
            actualTarget.second = defender->getPosition().second;

            newTarget.first = defender->getPosition().first + (defender->getPosition().first - attacker->getPosition().first);
            newTarget.second = defender->getPosition().second;

            defender->setPosition(newTarget);
            attacker->setPosition(actualTarget); //Pour l'instant, on considère que l'attaquant prend obligatoirement la place du défenseur. Il faudra coder une requête auprès de l'utilisateur.
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