#include "../mdlist.h"
#include <iostream>
#include <map>
#include <ctime>

#define MAX 100000
#define ULL unsigned long long

int main () 
{
    clock_t begin_time;

    // Starting test for map
    map<ULL, ULL> m;
    begin_time = clock();
    for (int i=0; i<MAX; i++)
        m[i] = i;
    cout << "Map finished INSERT in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds\n";
    begin_time = clock();
    for (int i=0; i<MAX; i++)
        int t = m[i];
    cout << "Map finished FIND in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds\n";

    // Starting test for MDList
    MDList<int> mdlist(8, 1LL<<32);
    begin_time = clock();
    for (int i=0; i<MAX; i++)
        mdlist.insert(i, i);
    cout << "MDList finished INSERT in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds\n";
    begin_time = clock();
    for (int i=0; i<MAX; i++)
        int t = mdlist.find(i);
    cout << "MDList finished FIND in " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << " seconds\n";
}