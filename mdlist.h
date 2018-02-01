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
#include <iostream>

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
    /**
     * Mutex lock for children.
     */
    std::mutex      child_mutex;
    /**
     * Mutex lock for value.
     */
    std::mutex      val_mutex;

    public:
                    Node(ULL, int, ULL, T val = NULL);
    void            lock();
    bool            try_lock();
    void            unlock();
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
 * Locks the node mutex.
 */
template <class T>
void Node<T>::lock () 
{
    this->mutex.lock();
}

/**
 * Non blocking lock operation on node mutex.
 * @returns True if locked else False.
 */
template <class T>
bool Node<T>::try_lock ()
{
    return this->mutex.try_lock();
}

/**
 * Unlocks the node mutex.
 */
template <class T>
void Node<T>::unlock ()
{
    this->mutex.unlock();
}

/**
 * Getter for key
 * @returns The key of Node.
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
    this->val_mutex.lock();
    this->val = val;
    this->val_mutex.unlock();
}

/**
 * Getter for Value.
 * @returns The value.
 */
template <class T>
T Node<T>::getValue ()
{
    this->val_mutex.lock();
    T t = this->val;
    this->val_mutex.unlock();
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
    this->child_mutex.lock();
    this->child[index] = childNode;
    this->child_mutex.unlock();
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
    this->child_mutex.lock();
    Node<T>* node = this->child[index];
    this->child_mutex.unlock();
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
    T                           remove(ULL);
    template <class _T>
    friend void                 printMDList(MDList<_T>);
    template <class _T>
    friend void                 findAndPrint(MDList<_T>, ULL);
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

std::mutex print;
/**
 * Insert (key, value) to MDList.
 * @param key The key.
 * @param val The value.
 */
template <class T>
void MDList<T>::insert (ULL key, T val)
{
    if (key < 0 || key >= N)
        return;
    start:
    vector<int> coordinates = keyToCoordinates(key, this->D, this->N);
    pair<Node<T>*, Node<T>*> p = locatePredecessor(coordinates);
    Node<T>* predecessor = p.first;
    Node<T>* current = p.second;
    // Lock for thread safety
    if (predecessor != NULL && !predecessor->try_lock())
        goto start;
    if (current != NULL && !current->try_lock())
    {
        if (predecessor != NULL)
            predecessor->unlock();
        goto start;
    }
    // Check predecessor and current are still valid
    p = locatePredecessor(coordinates);
    if (predecessor != p.first || current != p.second)
    {
        if (predecessor != NULL)
            predecessor->unlock();
        if (current != NULL)
            current->unlock();
        goto start;
    }
    // Check if key already exists.
    if (current != NULL && key == current->getKey())
    {
        // Update value and return.
        current->setValue(val);
        if (predecessor != NULL)
            predecessor->unlock();
        current->unlock();
        return;
    }
    // Key doesn't exits, create new Node.
    Node<T>* node = new Node<T>(key, this->D, this->N, val);
    // Find position of node in predecessor
    int d = 0;
    while (d < this->D && coordinates[d] <= predecessor->getCoordinates()[d])
        d++;
    if (d >= this->D)
        throw "Given key is out of key space";
    int _d = d;
    // If current is NULL, then node is _dth child of predecessor.
    if (current == NULL)
    {
        predecessor->setChild(_d, node);
        predecessor->unlock();
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
    predecessor->unlock();
    current->unlock();
}

/**
 * Searches for given key.
 * @param key The key.
 * @returns The value if key is present else NULL.
 */
template <class T>
T MDList<T>::find (ULL key) 
{
    if (key < 0 || key >= N)
        return NULL;
    vector<int> coordinates = keyToCoordinates(key, this->D, this->N);    
    pair<Node<T>*, Node<T>*> p = locatePredecessor(coordinates);
    Node<T>* current = p.second;
    if (current != NULL && current->getKey() == key)
        return current->getValue();
    return NULL;
}

/**
 * Removes the given key.
 * @param key The key.
 * @returns The value if key present and removed else NULL.
 */
template <class T>
T MDList<T>::remove(ULL key)
{
    if (key < 0 || key >= N)
        return NULL;
    // If given key is root
    // just remove the value
    // because root cannot be deleted.
    if (key == 0) 
    {
        T val = root->getValue();
        this->root->setValue(NULL);
        return val;
    }
    start_r:
    vector<int> coordinates = keyToCoordinates(key, this->D, this->N);    
    pair<Node<T>*, Node<T>*> p = locatePredecessor(coordinates);
    Node<T>* predecessor = p.first;
    Node<T>* current = p.second;
    // Lock for thread safety
    if (predecessor != NULL && !predecessor->try_lock())
        goto start_r;
    if (current != NULL && !current->try_lock())
    {
        if (predecessor != NULL)
            predecessor->unlock();
        goto start_r;
    }
    if (current == NULL || current->getKey() != key)
    {
        print.lock();
        // cout<<key<<" = "<<current->getKey()<<endl;
        print.unlock();
        // printMDList(*this);
        return NULL;
    }

    // Find the index of current in predecessor
    int d = 0;
    while (d < this->D && predecessor->getChild(d) != current)
        d++;
    if (d >= this->D)
        throw "Given key is out of key space";
    // The last indexed child of current will be the new current
    Node<T>* new_current = NULL;
    int _d = this->D;
    while (_d > 0 && new_current == NULL) {
        _d--;
        new_current = current->getChild(_d);
    }
    // Try lock for new_current
    if (new_current != NULL && !new_current->try_lock())
    {
        if (predecessor != NULL)
            predecessor->unlock();
        if (current != NULL)
            current->unlock();
        goto start_r;
    }
    // Check predecessor and current are still valid
    p = locatePredecessor(coordinates);
    if (predecessor != p.first || current != p.second || 
            (current != NULL && current->getKey() != key))
    {
        if (predecessor != NULL)
            predecessor->unlock();
        if (current != NULL)
            current->unlock();
        if (new_current != NULL)
            new_current->unlock();
        goto start_r;
    }
    // Transer children of current to new current
    _d--;
    while (_d >= 0) {
        new_current->setChild(_d, current->getChild(_d));
        _d--;
    }
    // Update predecessor pointer
    predecessor->setChild(d, new_current);
    if (predecessor)
        predecessor->unlock();
    if (new_current)
        new_current->unlock();
    T val = current->getValue();
    delete current;
    return val;
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

/**
 * Finds and prints node for given key.
 * Only for debug purposes.
 * @param mdlist The mdlist.
 * @param key The key.
 */
template <class T>
void findAndPrint (MDList<T> mdlist, ULL key) 
{
    vector<int> coordinates = keyToCoordinates(key, mdlist.D, mdlist.N);    
    pair<Node<T>*, Node<T>*> p = mdlist.locatePredecessor(coordinates);
    Node<T>* node = p.second;
    if (node->getKey() == key)
    {
        cout<<node->getKey()<<" ";
        vector<int> coordinates = node->getCoordinates();
        cout<<"[";
        for (int d = 0; d < mdlist.D; d++)
        {
            cout<<coordinates[d]<<", ";
        }
        cout<<"] : ";
        if (node->getValue() != NULL)
            cout<<node->getValue()<<"\n\t";
        else
            cout<<"NULL\n\t";
        for (int d = 0; d < mdlist.D; d++)
        {
            Node<T>* child = node->getChild(d);
            cout<<d<<" : ";
            if (child == NULL)
                cout<<"NULL , ";
            else 
            {
                vector<int> coordinates = child->getCoordinates();
                cout<<"[";
                for (int d = 0; d < mdlist.D; d++)
                {
                    cout<<coordinates[d]<<", ";
                }
                cout<<"] , ";
            }
        }
        cout<<endl;
    } 
    else 
    {
        cout << key << " Not found!\n";
    }
}

#endif