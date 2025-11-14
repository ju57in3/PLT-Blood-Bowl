#include "Block.h"

namespace engine {
    Block::Block(state::Character attacker, state::Character defender):attacker(attacker), defender(defender){}

    BlockResult Block::rollBlockDice()
    {
        int diceResult;
        int range = 6; //Range lancé de Dé pour le Block
        int roll1 = rollDice(6);
        diceResult = roll1; //Si l'attaquant et le défenseur ont le même nombre de points de force, il n'y a qu'un seul lancé de dé

        if (attacker.getStrength()>= defender.getStrength())
        {
            int roll2 = rollDice(6);
            diceResult = std::max(roll1,roll2);  // Le choix arrangeant le plus l'attaquant sont les résultats élevés
            if (attacker.getStrength()>= 2*defender.getStrength())
            {
                int roll3 = rollDice(6);
                diceResult = std::max(roll3,diceResult);
            }
        }

        else if (attacker.getStrength() <= defender.getStrength())
        {
            int roll2 = rollDice(6);
            diceResult = std::min(roll1,roll2); // Le choix arrangeant le plus le défenseur sont les résultats faibles
            if (attacker.getStrength()>= 2*defender.getStrength())
            {
                int roll3 = rollDice(6);
                diceResult = std::min(roll3,diceResult);
            }
        }

        if (diceResult == 1)
        {
            return AttackerDown;
        }
        if (diceResult == 2){
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

};