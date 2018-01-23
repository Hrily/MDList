#include <iostream>
#include "../mdlist.h"

using namespace std;

int main () {
    MDList<int> mdlist(2, 9);

    mdlist.insert(4, 1);
    printMDList(mdlist);
    cout<<endl;

    mdlist.insert(3, 1);
    printMDList(mdlist);
    cout<<endl;

    mdlist.insert(8, 1);
    printMDList(mdlist);
    cout<<endl;

    mdlist.insert(2, 1);
    printMDList(mdlist);
    cout<<endl;

    mdlist.insert(1, 1);
    printMDList(mdlist);
    cout<<endl;

    mdlist.insert(7, 1);
    mdlist.insert(5, 1);
    printMDList(mdlist);
    cout<<endl;
}