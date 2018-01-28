#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE NodeTest
#include <boost/test/unit_test.hpp>
#include "../mdlist.h"

BOOST_AUTO_TEST_SUITE(MDListFindTest)

BOOST_AUTO_TEST_CASE(Test1) {
    MDList<int> mdlist(3, 64);
    for (int i = 0; i < 64; i++)
        mdlist.insert(i, i);
    for (int i = 0; i < 64; i++)
        BOOST_CHECK_EQUAL(i, mdlist.find(i));
}

BOOST_AUTO_TEST_SUITE_END()