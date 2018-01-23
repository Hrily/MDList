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
    int i = 0;
    while (key > 0 && i < D) {
        coordinates[i++] = key % M;
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

#endif