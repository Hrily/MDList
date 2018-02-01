#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MDListTest
#include <boost/test/unit_test.hpp>
#include <vector>
#include <thread>
#include <algorithm>
#include "../mdlist.h"
using namespace std;

#define RANGE 1000
#define N_THREADS 4

MDList<int> mdlist(8, 1LL << 32);
vector<int> pool;
// MDList<int> mdlist(2, 9);

void insertElements (int n)
{
    for (int i = n*RANGE; i < (n+1)*RANGE; i++)
    {
        print.lock();
        // cout << n << " : Deleting " << pool[i] << endl;
        print.unlock();
        int f = mdlist.remove(pool[i]);
        print.lock();
        // cout << n << " : Deleted " << pool[i] << " : "  << f << endl;
        print.unlock();
    }
}

BOOST_AUTO_TEST_SUITE(MDListConcurrentDeleteTest)

BOOST_AUTO_TEST_CASE(Test1) {
    for (int i = 1; i < N_THREADS*RANGE; i++)
    {
        mdlist.insert(i, i);
        pool.push_back(i);
    }
    random_shuffle(pool.begin(), pool.end());
    vector<thread> threads(N_THREADS);
    for (int i=0; i<N_THREADS; i++) 
        threads[i] = thread(insertElements, i);
    for (int i=0; i<N_THREADS; i++) 
        threads[i].join();
    for (int i=1; i < N_THREADS*RANGE; i++)
        BOOST_CHECK_EQUAL(NULL,  mdlist.find(i));
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