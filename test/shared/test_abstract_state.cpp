//
// Created by justine on 16/10/2025.
//

#include <boost/test/unit_test.hpp>
#include "state.h"

using namespace state;

class DummyState : public AbstractState {
    public:
    explicit DummyState(BloodBowlGame* game) : AbstractState(game) {}
};

BOOST_AUTO_TEST_CASE(TestAbstractState)
{
    Team teamA(1, "Humans", 3);
    Team teamB(2, "Orcs", 2);
    BloodBowlGame game(teamA, teamB);

    // Create a concrete instance that uses base implementations
    DummyState dummy(&game);
    dummy.update ();

    // Cover AbstractState::getName() (returns "AbstractState")
    std::string nameDirect = dummy.getName();
    BOOST_CHECK_EQUAL(nameDirect, "AbstractState");

    // Also call through the base pointer to confirm virtual dispatch uses base impl
    AbstractState* basePtr = &dummy;
    std::string nameViaBase = basePtr->getName();
    BOOST_CHECK_EQUAL(nameViaBase, "AbstractState");

    // Sanity: game pointer should still be valid
    BOOST_CHECK(basePtr != nullptr);
}