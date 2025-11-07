#include "Client.h"

using namespace state;
namespace client
{
    Client::Client()
    {
        Team teamA(1, "Humans", 3);
        Team teamB(2, "Orcs", 2);

        auto hum1 = std::make_shared<Character>("Ligne1", "Human", 6, 3, 3, 8);
        teamA.addCharacter(hum1);
        hum1->setPosition({12, 6});
        hum1->setStatus(playable);

        auto hum2 = std::make_shared<Character>("Ligne2", "Human", 6, 3, 3, 8);
        teamA.addCharacter(hum2);
        hum2->setPosition({12, 7});
        hum2->setStatus(playable);

        auto hum3 = std::make_shared<Character>("Ligne3", "Human", 6, 3, 3, 8);
        teamA.addCharacter(hum3);
        hum3->setPosition({12, 8});
        hum3->setStatus(playable);

        auto hum4 = std::make_shared<Character>("Bottom1", "Human", 6, 3, 3, 8);
        teamA.addCharacter(hum4);
        hum4->setPosition({10, 2});
        hum4->setStatus(playable);

        auto orc1 = std::make_shared<Character>("Ligne1", "orc", 6, 3, 3, 8);
        teamB.addCharacter(orc1);
        orc1->setPosition({13, 6});
        orc1->setStatus(playable);

        auto orc2 = std::make_shared<Character>("Ligne2", "orc", 6, 3, 3, 8);
        teamB.addCharacter(orc2);
        orc2->setPosition({13, 7});
        orc2->setStatus(playable);

        auto orc3 = std::make_shared<Character>("Ligne3", "orc", 6, 3, 3, 8);
        teamB.addCharacter(orc3);
        orc3->setPosition({13, 8});
        orc3->setStatus(playable);

        auto orc4 = std::make_shared<Character>("Bottom1", "orc", 6, 3, 3, 8);
        teamB.addCharacter(orc4);
        orc4->setPosition({13, 2});
        orc4->setStatus(playable);

        this->game = std::make_shared<BloodBowlGame>(BloodBowlGame(teamA, teamB));


    }
}
