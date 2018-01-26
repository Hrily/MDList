/**
 * @file mdlist.h
 * @brief An implementation of "An Efficient Lock-free Logarithmic 
 * Search Data Structure Based on Multi-dimensional List".
 * From 2016 IEEE 36th International Conference on Distributed 
 * Computing Systems.
 * 
 * @author Hrishikesh Hiraskar
 */

#ifndef _mdlist_h_
#define _mdlist_h_

#include <vector>
#include <mutex>
#include <cmath>
#include <exception>

#define ULL unsigned long long

using namespace std;

// Hepler Functions

/**
 * Finds nth root of given number.
 * @param X The number.
 * @param n The root.
 * @returns The nth root.
 */
long nthRoot (ULL X, int n)
{
    long nth_root = trunc(std::pow(X, 1.0 / n));
    // because of rounding error, 
    // it's possible that nth_root + 1 
    // is what we actually want; let's check
    if (std::pow(nth_root + 1, n) <= X) {
        return nth_root + 1;
    }
    return nth_root;
}

/**
 * Converts key to node coordinates.
 * @param key The key.
 * @param D D value of MDList.
 * @param N The key space.
 * @returns Vector of coordinates.
 */
vector<int> keyToCoordinates (ULL key, int D, ULL N)
{
    int M = nthRoot(N, D);
    vector<int> coordinates(D);
    int i = D-1;
    while (key > 0 && i >= 0) {
        coordinates[i--] = key % M;
        key /= M;
    }
    return coordinates;
}

/**
 * Node class.
 */
template <class T>
class Node
{
    /**
     * The key.
     */
    ULL             key;
    /**
     * The value.
     */
    T               val;
    /**
     * The Node coordinates.
     * This is constant for a given Node.
     */
    vector<int>     COORDINATES;
    /**
     * The vector of children of this Node.
     */
    vector<Node*>   child;
    /**
     * Mutex lock for this Node.
     */
    std::mutex      mutex;

    public:
                    Node(ULL, int, ULL, T val = NULL);
    ULL             getKey();
    void            setValue(T);
    T               getValue();
    void            setChild(int, Node*);
    Node<T>*        getChild(int);
    vector<int>     getCoordinates();
};

/**
 * Node Class Constructor.
 * @param key The key.
 * @param D D value of MDList.
 * @param N The key space.
 * @param val The value (optional).
 */
template <class T>
Node<T>::Node (ULL key, int D, ULL N, T val) 
{
    this->key = key;
    this->val = val;
    this->COORDINATES = keyToCoordinates(key, D, N);
    this->child.assign(D, NULL);
}

/**
 * Getter for key
 * @returns The key of Node.\
 */
template <class T>
ULL Node<T>::getKey ()
{
    // Constant so no need for lock
    return this->key;
}

/**
 * Setter for value.
 * @param val The value.
 */
template <class T>
void Node<T>::setValue (T val)
{
    this->mutex.lock();
    this->val = val;
    this->mutex.unlock();
}

/**
 * Getter for Value.
 * @returns The value.
 */
template <class T>
T Node<T>::getValue ()
{
    this->mutex.lock();
    T t = this->val;
    this->mutex.unlock();
    return t;
}

/**
 * Setter for child Node.
 * @param index The index of Child.
 * @param childNode The child Node.
 */
template <class T>
void Node<T>::setChild (int index, Node* childNode)
{
    if (index < 0 && index >= this->child.size())
        throw "Index out of bounds";
    this->mutex.lock();
    this->child[index] = childNode;
    this->mutex.unlock();
}

/**
 * Getter for Child.
 * @param index The index of child.
 * @returns The child.
 */
template <class T>
Node<T>* Node<T>::getChild (int index)
{
    if (index < 0 && index >= this->child.size())
        throw "Index out of bounds";
    this->mutex.lock();
    Node<T>* node = this->child[index];
    this->mutex.unlock();
    return node;
}

/**
 * Getter for Node coordinates.
 * @returns The Node coordinates.
 */
template <class T>
vector<int> Node<T>::getCoordinates ()
{
    return this->COORDINATES;
}


/**
 * MDList class
 */
template <class T>
class MDList
{
    /**
     * The D value of MDList.
     */
    int                         D;
    /**
     * The key space.
     */
    ULL                         N;
    /**
     * Head or Root of the MDList.
     */
    Node<T>*                    root;
    pair<Node<T>*, Node<T>*>    locatePredecessor(vector<int>);
    public:
                                MDList(int, ULL);
    void                        insert(ULL, T);
    T                           find(ULL);
    void                        remove(ULL);
    template <class _T>
    friend void                 printMDList(MDList<_T>);
};

/**
 * MDList constructor.
 * @param D The D value of MDList.
 * @param N The key space.
 */
template <class T>
MDList<T>::MDList (int D, ULL N)
{
    this->D = D;
    this->N = N;
    this->root = new Node<T>(0, D, N);
}

/**
 * Locate Predecessor/Parent of given coordinates.
 * @param coordinates The given coordinates.
 * @returns (predecessor, childIndex) if found else NULL
 *          where childIndex is the index where the child is.
 */
template <class T>
pair<Node<T>*, Node<T>*> MDList<T>::locatePredecessor (vector<int> coordinates)
{
    Node<T>* current = this->root;
    Node<T>* predecessor = NULL;
    int d = 0;
    while (d < this->D)
    {
        while (current != NULL && coordinates[d] > current->getCoordinates()[d]){
            predecessor = current;
            current = current->getChild(d);
        }
        // Check if we found the predecessor.
        if (current == NULL || coordinates[d] < current->getCoordinates()[d])
            break;
        else
            d++;
    }
    return make_pair(predecessor, current);
}

/**
 * Insert (key, value) to MDList.
 * @param key The key.
 * @param val The value.
 */
template <class T>
void MDList<T>::insert (ULL key, T val)
{
    vector<int> coordinates = keyToCoordinates(key, this->D, this->N);
    pair<Node<T>*, Node<T>*> p = locatePredecessor(coordinates);
    Node<T>* predecessor = p.first;
    Node<T>* current = p.second;
    // Check if key already exists.
    if (current != NULL && key == current->getKey())
    {
        // Update value and return.
        current->setValue(val);
        return;
    }
    // Key doesn't exits, create new Node.
    Node<T>* node = new Node<T>(key, this->D, this->N, val);
    int d = 0;
    while (d < this->D && coordinates[d] <= predecessor->getCoordinates()[d])
        d++;
    if (d >= this->D)
        throw "Given key is out of key space";
    int _d = d;
    // If current is NULL, then node is dth child of predecessor.
    if (current == NULL)
    {
        predecessor->setChild(d, node);
        return;
    }
    // Assign appropriate childrens to node.
    while (d < this->D)
    {
        // Check if current can be dth child of node.
        if (node->getCoordinates()[d] < current->getCoordinates()[d]){
            node->setChild(d, current);
            break;
        }
        // Else dth child of current is dth child of node
        node->setChild(d, current->getChild(d));
        current->setChild(d, NULL);
        d++;
    }
    // Assign predecessor as parent to node.
    predecessor->setChild(_d, node);
}

/**
 * Prints MDList.
 * Only for debug purposes.
 * @param mdList The MDList to print
 */
template <class T>
void printMDList (MDList<T> mdList)
{
    vector< Node<T>* > stack;
    stack.push_back(mdList.root);
    while(stack.size() > 0)
    {
        Node<T>* node = stack.back();
        stack.pop_back();
        cout<<node->getKey()<<" ";
        vector<int> coordinates = node->getCoordinates();
        cout<<"[";
        for (int d = 0; d < mdList.D; d++)
        {
            cout<<coordinates[d]<<", ";
        }
        cout<<"] : ";
        if (node->getValue() != NULL)
            cout<<node->getValue()<<"\n\t";
        else
            cout<<"NULL\n\t";
        for (int d = 0; d < mdList.D; d++)
        {
            Node<T>* child = node->getChild(d);
            cout<<d<<" : ";
            if (child == NULL)
                cout<<"NULL , ";
            else 
            {
                stack.push_back(child);
                vector<int> coordinates = child->getCoordinates();
                cout<<"[";
                for (int d = 0; d < mdList.D; d++)
                {
                    cout<<coordinates[d]<<", ";
                }
                cout<<"] , ";
            }
        }
        cout<<endl;
    }
}

#endif