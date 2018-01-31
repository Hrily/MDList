#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MDListTest
#include <boost/test/unit_test.hpp>
#include <vector>
#include <thread>
#include <iostream>
#include "../mdlist.h"
using namespace std;

#define N_THREADS 500

Node<int> node(1234, 8, 1ULL << 32, 0);

void increement (int tid)
{
    node.lock();
    // cout << "Tid " << tid << " got lock\n";
    int r = node.getValue();
    r++;
    // cout<<r<<endl;
    node.setValue(r);
    node.unlock();
    // cout << "Tid " << tid << " got unlock\n";
}

BOOST_AUTO_TEST_SUITE(MDListFindTest)

BOOST_AUTO_TEST_CASE(Test1) {
    vector<thread> threads(N_THREADS);
    for (int i=0; i<N_THREADS; i++) 
        threads[i] = thread(increement, i);
    for (int i=0; i<N_THREADS; i++) 
        threads[i].join();
    // cout<<"Finally "<<node.getValue()<<endl;
    BOOST_CHECK_EQUAL(N_THREADS, node.getValue());
}

BOOST_AUTO_TEST_SUITE_END()