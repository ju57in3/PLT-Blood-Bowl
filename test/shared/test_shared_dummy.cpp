
#include <boost/test/unit_test.hpp>

#include "../../src/shared/state/Example.h"
#include "../../src/shared/state/ExampleA.h"
#include "../../src/shared/state/ExampleB.h"

using namespace ::state;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestExemple)
{
  {
    ExampleA exA;
    exA.setX(53);
    Example ex;
    ex.setA(exA);
    BOOST_CHECK_EQUAL(ex.getA().x, 53);
    exA.setX(21);
    BOOST_CHECK_EQUAL(ex.getA().x, 53);
    ex.setA(exA);
    BOOST_CHECK_EQUAL(ex.getA().x, 21);
  }

  {
    ExampleA exA;
    exA.setX(21);
    Example ex;
    ex.setA(exA);
    BOOST_CHECK_LE(ex.getA().x, 32); // Less than equal
    BOOST_CHECK_GT(ex.getA().x, 11); // Greater than equal
  }
}

/* vim: set sw=2 sts=2 et : */
