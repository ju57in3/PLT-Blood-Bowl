//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include "engine/Pass.h"

using namespace engine;
using namespace state;

BOOST_AUTO_TEST_CASE(TestAbstractState) {
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto hum = std::make_shared<Character>(1,"human", "human_blitzer", 6, 4, 3, 8);
    teamA.addCharacter(hum);
    auto orc = std::make_shared<Character>(2,"orc", "orc", 6, 4, 3, 8);
    teamA.addCharacter(orc);

    auto passCmd = std::make_unique<engine::Pass>(hum, orc);


}