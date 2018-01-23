#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE NodeTest
#include <boost/test/unit_test.hpp>
#include "../mdlist.h"

BOOST_AUTO_TEST_SUITE(NodeTest)

BOOST_AUTO_TEST_CASE(coordinatesTest) {
    Node<int> node(1234, 8, 1ULL << 32);
    vector<int> coordinates = node.getCoordinates();
    int original_coordinates[8] = {2, 13, 4, 0, 0, 0, 0, 0};
    for (int i=0; i<8; i++)
        BOOST_CHECK_EQUAL(original_coordinates[i], coordinates[i]);
}

BOOST_AUTO_TEST_SUITE_END()