#include "../mdlist.h"
#include <iostream>
#include <map>
#include <ctime>

#define MAX 100000

int main () 
{
    clock_t begin_time;

    // Starting test for map
    map<int, int> m;
    begin_time = clock();
    for (int i=0; i<MAX; i++)
    {
        m[i] = i;
        int t = m[i];
    }
    cout << "Map finished in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds\n";

    // Starting test for MDList
    MDList<int> mdlist(8, 1LL<<32);
    begin_time = clock();
    for (int i=0; i<MAX; i++)
    {
        mdlist.insert(i, i);
        int t = mdlist.find(i);
    }
    cout << "MDList finished in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds\n";
}