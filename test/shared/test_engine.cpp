//
// Created by matt-o on 1/9/26.
//
#include <boost/test/unit_test.hpp>
#include "engine/Engine.h"

using namespace engine;
using namespace state;

BOOST_AUTO_TEST_CASE(TestEngine) {
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);

    auto gamePtr = std::make_shared<BloodBowlGame>(teamA, teamB);
    Engine engine(gamePtr);

    BOOST_CHECK(engine.getGameState() != nullptr);
}