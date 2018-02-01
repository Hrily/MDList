#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MDListTest
#include <boost/test/unit_test.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include "../mdlist.h"
using namespace std;

#define RANGE 1000
#define N_THREADS 4

MDList<int> mdlist(8, 1LL << 32);
// MDList<int> mdlist(3, 64);
// mutex print;

void insertElements (int n)
{
    for (int i = n*RANGE; i < (n+1)*RANGE; i++)
    {
        // print.lock();
        // cout << n << " : Adding " << i << endl;
        // print.unlock();
        mdlist.insert(i, i);
        // print.lock();
        // cout << n << " : Added " << i << endl;
        // print.unlock();
    }
}

BOOST_AUTO_TEST_SUITE(MDListConcurrentInsertTest)

BOOST_AUTO_TEST_CASE(Test1) {
    vector<thread> threads(N_THREADS);
    for (int i=0; i<N_THREADS; i++) 
        threads[i] = thread(insertElements, i);
    for (int i=0; i<N_THREADS; i++) 
        threads[i].join();
    for (int i=0; i < N_THREADS*RANGE; i++)
        BOOST_CHECK_EQUAL(i,  mdlist.find(i));
    // for (int i=0; i<N_THREADS*RANGE; i++){
    //     if (i != mdlist.find(i)){
    //         cout<< i << " != " << mdlist.find(i) << endl;
    //         findAndPrint(mdlist, i-1);
    //         findAndPrint(mdlist, i);
    //         findAndPrint(mdlist, i+1);
    //         cout << endl << endl;
    //     }
    // }
    // printMDList(mdlist);
}

BOOST_AUTO_TEST_SUITE_END()