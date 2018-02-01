# MDList

An implementation of "An Efficient Lock-free Logarithmic Search Data Structure Based on Multi-dimensional List". From 2016 IEEE 36th International Conference on Distributed Computing Systems.

**MDList** is a dictionary based data-structure which stores (key, value) pairs. The good thing about MDList is, it is lock-free, i.e. thread safe by default.

MDList stores (key, value) pairs in form of Nodes. Each node has a (key, value) pair and coordinates in key space. More information on MDList can be found in the paper.

## Usage

First, include the header file in your code:
```
#include "mdlist.h"
```

### Initialization

The current implementation of MDList can store key on integer type (`unsigned long long` to be specific), but can store value of any type.

To init a MDList which stores `string` values:
```
MDList<string> mdlist(8, 1LL << 32)
```
Where `(8, 1LL << 32)` are D, N constants of MDList. (See Paper for info on these constants.)

### Insert

Insertion can be done as follows
```
mdlist.insert(<key>, <value>);
```
For example:
```
mdlist.insert(1, "hello");
```

### Find

To find key `1` :
```
string value = mdlist.find(1);
```
This will return value if key exists or `NULL`.

### Remove

To remove `1`:
```
string value = mdlist.remove(1)
```
This will return value if key exists or `NULL`.


## Documentation

The documentation for the source code can be found [here](https://hrily.github.io/MDList).

The documentation was made with help of Doxygen.

## Performance and Limitations

On comparision with `std::map` of C++ STL, Map outperforms MDList by large fraction. I'll be working on improving the run times. Meanwhile, any PRs are most welcome :).

But in multi-threaded environment, MDList takes less time than Map, which is a good thing.