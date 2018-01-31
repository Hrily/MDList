#include <iostream>
#include "../mdlist.h"

using namespace std;

int main () {
    MDList<int> mdlist(3, 64);

    mdlist.insert(18, 1);
    mdlist.insert(33, 1);

    mdlist.insert(4, 1);
    mdlist.insert(6, 1);

    mdlist.insert(1, 1);
    mdlist.insert(2, 1);

    mdlist.insert(19, 1);
    mdlist.insert(22, 1);

    mdlist.insert(34, 1);
    mdlist.insert(36, 1);

    mdlist.insert(48, 1);
    
    printMDList(mdlist);
    cout<<endl<<endl;

    mdlist.remove(18);
    mdlist.remove(48);
    printMDList(mdlist);
}