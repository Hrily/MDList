#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MDListTest
#include <boost/test/unit_test.hpp>
#include "../mdlist.h"

#define D (3)
#define N (64)

BOOST_AUTO_TEST_SUITE(MDListDeleteTest)

BOOST_AUTO_TEST_CASE(Test1) {
    MDList<int> mdlist(D, N);
    for (int i = 0; i < N; i++)
        mdlist.insert(i, i);
    for (int i = 0; i < N; i++)
        BOOST_CHECK_EQUAL(i, mdlist.remove(i));
    BOOST_CHECK_EQUAL(NULL, mdlist.remove(60));
    BOOST_CHECK_EQUAL(NULL, mdlist.remove(65));
}

BOOST_AUTO_TEST_SUITE_END()