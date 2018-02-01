#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <algorithm>
#include <ctime>
#include "../mdlist.h"
using namespace std;

#define ULL unsigned long long

#define RANGE 1000
#define N_THREADS 8

vector<ULL> pool;
map<ULL, ULL> m;
mutex map_mutex;
MDList<ULL> mdlist(8, 1LL << 32);

void insertToMap (int n)
{
    for (int i = n*RANGE; i < (n+1)*RANGE; i++)
    {
        map_mutex.lock();
        m[i] = pool[i];
        map_mutex.unlock();
    }
}

void insertToMDList (int n)
{
    for (int i = n*RANGE; i < (n+1)*RANGE; i++)
        mdlist.insert(pool[i], pool[i]);    
}

int main ()
{
    clock_t begin_time;
    vector<thread> map_threads(N_THREADS),
                   mdlist_threads(N_THREADS);

    // Init pool
    for (int i=0; i<N_THREADS*RANGE; i++)
        pool.push_back(i);
    random_shuffle(pool.begin(), pool.end());
    
    // Start Map threads
    cout << "Starting Map Threads\n";
    begin_time = clock();
    for (int i=0; i<N_THREADS; i++)
        map_threads[i] = thread(insertToMap, i);
    for (int i=0; i<N_THREADS; i++)
        map_threads[i].join();        
    cout << "Map finished INSERT in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds\n";

    // Start MDList threads
    cout << "Starting MDList Threads\n";
    begin_time = clock();
    for (int i=0; i<N_THREADS; i++)
        mdlist_threads[i] = thread(insertToMap, i);
    for (int i=0; i<N_THREADS; i++)
        mdlist_threads[i].join();        
    cout << "MDList finished INSERT in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds\n";
}