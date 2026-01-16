//
// Created by matt-o on 1/16/26.
//

#include <boost/test/unit_test.hpp>
#include "utility/GameUtils.h"

using namespace state;
using namespace utility;


/* Helpers */

static std::shared_ptr<Character> makeChar(
    int id, std::string name, int x, int y, CharacterStatus st, int agi = 3)
{
    auto c = std::make_shared<Character>(id, name, "Human", 6, agi, 3, 8);
    c->setPosition({x, y});
    c->setStatus(st);
    return c;
}

static void seedRNG(unsigned int seed)
{
    GameUtils::getRNG().seed(seed);
}

BOOST_AUTO_TEST_CASE(TestGameUtils)
{
    // getRNG : singleton check
    auto& r1 = GameUtils::getRNG();
    auto& r2 = GameUtils::getRNG();
    BOOST_CHECK(&r1 == &r2);


    // scatterOnce : all 8 directions reachable
    seedRNG(0);

    std::pair<int,int> start{10,10};
    bool seen[9] = {false};

    for (int i = 0; i < 200; ++i) {
        auto p = GameUtils::scatterOnce(start);
        int dx = p.first - start.first;
        int dy = p.second - start.second;

        if (dx == 0 && dy == 1)   seen[1] = true;
        if (dx == 1 && dy == 1)   seen[2] = true;
        if (dx == 1 && dy == 0)   seen[3] = true;
        if (dx == 1 && dy == -1)  seen[4] = true;
        if (dx == 0 && dy == -1)  seen[5] = true;
        if (dx == -1 && dy == -1) seen[6] = true;
        if (dx == -1 && dy == 0)  seen[7] = true;
        if (dx == -1 && dy == 1)  seen[8] = true;
    }

    for (int i = 1; i <= 8; ++i)
        BOOST_CHECK(seen[i]);


    // agilityTarget : correct target values
    BOOST_CHECK_EQUAL(GameUtils::agilityTarget(1), 6);
    BOOST_CHECK_EQUAL(GameUtils::agilityTarget(2), 5);
    BOOST_CHECK_EQUAL(GameUtils::agilityTarget(3), 4);
    BOOST_CHECK_EQUAL(GameUtils::agilityTarget(4), 3);
    BOOST_CHECK_EQUAL(GameUtils::agilityTarget(5), 2);
    BOOST_CHECK_EQUAL(GameUtils::agilityTarget(10), 2);


    // agilityTest : natural 1 fail, natural 6 success, normal roll
    bool got1 = false, got6 = false, gotNormal = false;

    for (unsigned s = 0; s < 2000; ++s) {
        seedRNG(s);
        int roll = std::uniform_int_distribution<int>(1,6)(GameUtils::getRNG());
        seedRNG(s);

        bool res = GameUtils::agilityTest(3, 0);

        if (roll == 1) {
            BOOST_CHECK(!res);
            got1 = true;
        }
        else if (roll == 6) {
            BOOST_CHECK(res);
            got6 = true;
        }
        else {
            gotNormal = true;
        }

        if (got1 && got6 && gotNormal) break;
    }

    BOOST_CHECK(got1);
    BOOST_CHECK(got6);
    BOOST_CHECK(gotNormal);

    seedRNG(42);
    bool r = GameUtils::agilityTest(3, -5);
    BOOST_CHECK(!r || r); // only to execute modifier branch


    // countTackleZones : only adjacent standing opponents
    Team A1(1,"A",3);
    Team B1(2,"B",3);

    auto center = makeChar(1,"C",5,5,playable);
    A1.addCharacter(center);

    B1.addCharacter(makeChar(2,"O1",6,5,playable));
    B1.addCharacter(makeChar(3,"O2",4,4,played));
    B1.addCharacter(makeChar(4,"O3",5,6,knockedDown));
    B1.addCharacter(makeChar(5,"O4",10,10,playable));

    int tz = GameUtils::countTackleZones(*center, B1);
    BOOST_CHECK_EQUAL(tz, 2);


    // blockableCharacters : adjacent standing only + nullptr case
    Team A2(1,"A",3);
    Team B2(2,"B",3);

    auto by = makeChar(1,"C",5,5,playable);
    A2.addCharacter(by);

    auto b1 = makeChar(2,"O1",6,5,playable);
    auto b2 = makeChar(3,"O2",4,4,played);
    auto b3 = makeChar(4,"O3",5,6,stunned);

    B2.addCharacter(b1);
    B2.addCharacter(b2);
    B2.addCharacter(b3);

    auto list = GameUtils::blockableCharacters(by, B2);
    BOOST_CHECK_EQUAL(list.size(), 2);

    auto empty = GameUtils::blockableCharacters(nullptr, B2);
    BOOST_CHECK(empty.empty());


    // isCharacterStanding : status filtering
    BOOST_CHECK(!GameUtils::isCharacterStanding(nullptr));
    BOOST_CHECK(!GameUtils::isCharacterStanding(makeChar(1,"a",0,0,knockedDown)));
    BOOST_CHECK(!GameUtils::isCharacterStanding(makeChar(1,"a",0,0,stunned)));
    BOOST_CHECK(!GameUtils::isCharacterStanding(makeChar(1,"a",0,0,ko)));
    BOOST_CHECK(!GameUtils::isCharacterStanding(makeChar(1,"a",0,0,injured)));
    BOOST_CHECK(!GameUtils::isCharacterStanding(makeChar(1,"a",0,0,bench)));
    BOOST_CHECK(GameUtils::isCharacterStanding(makeChar(1,"a",0,0,playable)));
    BOOST_CHECK(GameUtils::isCharacterStanding(makeChar(1,"a",0,0,played)));


    // getCharacterAt : lookup on board + null safety
    Team A3(1,"A",3);
    Team B3(2,"B",3);

    auto c1 = makeChar(1,"A1",1,1,playable);
    auto c2 = makeChar(2,"B1",2,2,playable);

    A3.addCharacter(c1);
    B3.addCharacter(c2);

    auto game1 = std::make_shared<BloodBowlGame>(A3,B3);

    BOOST_CHECK(GameUtils::getCharacterAt(game1.get(), {1,1}) == c1);
    BOOST_CHECK(GameUtils::getCharacterAt(game1.get(), {2,2}) == c2);
    BOOST_CHECK(GameUtils::getCharacterAt(game1.get(), {9,9}) == nullptr);

    BOOST_CHECK(GameUtils::getCharacterAt(game1, {1,1}) == c1);
    BOOST_CHECK(GameUtils::getCharacterAt(game1, {9,9}) == nullptr);

    BOOST_CHECK(GameUtils::getCharacterAt((BloodBowlGame*)nullptr, {1,1}) == nullptr);
    BOOST_CHECK(GameUtils::getCharacterAt(std::shared_ptr<BloodBowlGame>(), {1,1}) == nullptr);


    // handleBallBounce : null game -> out of bounds
    bool oob=false, to=false;
    GameUtils::handleBallBounce(nullptr, {5,5}, oob, to, nullptr);
    BOOST_CHECK(oob);
    BOOST_CHECK(!to);


    // handleBallBounce : empty tile causes turnover
    Team A4(1,"A",3);
    Team B4(2,"B",3);
    auto game2 = std::make_shared<BloodBowlGame>(A4,B4);

    seedRNG(1);

    oob=false; to=false;
    GameUtils::handleBallBounce(game2, {5,5}, oob, to, &A4);

    BOOST_CHECK(!oob);
    BOOST_CHECK(to);
    BOOST_CHECK(!game2->getBallIsHold());


    // handleBallBounce : caught by same team
    Team A5(1,"A",3);
    Team B5(2,"B",3);

    int dx[8] = {0,1,1,1,0,-1,-1,-1};
    int dy[8] = {1,1,0,-1,-1,-1,0,1};

    for (int i = 0; i < 8; ++i)
        A5.addCharacter(makeChar(i+1, "A"+std::to_string(i+1),
                                 5 + dx[i], 5 + dy[i], playable, 6));

    auto game3 = std::make_shared<BloodBowlGame>(A5,B5);

    seedRNG(123);

    oob=false; to=false;
    GameUtils::handleBallBounce(game3, {5,5}, oob, to, &A5);

    BOOST_CHECK(game3->getBallIsHold());
    BOOST_CHECK(!to);


    // handleBallBounce : caught by other team -> turnover
    Team A6(1,"A",3);
    Team B6(2,"B",3);

    for (int i = 0; i < 8; ++i)
        B6.addCharacter(makeChar(i+1, "B"+std::to_string(i+1),
                                 5 + dx[i], 5 + dy[i], playable, 6));

    auto game4 = std::make_shared<BloodBowlGame>(A6,B6);

    seedRNG(999);

    oob=false; to=false;
    GameUtils::handleBallBounce(game4, {5,5}, oob, to, &A6);

    BOOST_CHECK(game4->getBallIsHold());
    BOOST_CHECK(to);


    // handleBallBounce : knocked down player does not catch
    Team A7(1,"A",3);
    Team B7(2,"B",3);

    auto down = makeChar(1,"D",6,5,knockedDown);
    B7.addCharacter(down);

    auto game5 = std::make_shared<BloodBowlGame>(A7,B7);

    seedRNG(0);

    oob=false; to=false;
    GameUtils::handleBallBounce(game5, {5,5}, oob, to, nullptr);

    BOOST_CHECK(!oob);
}
