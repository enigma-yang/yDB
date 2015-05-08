#ifndef _BPLUSTREE_HPP
#define _BPLUSTREE_HPP

#include <iostream>
#include <stack>
#include <map>
#include <string>
#include <semaphore.h>

#include "rtm.h"

using namespace std;

class BPlusTree
{
private:
    static const int minimumDegree = 2;
    static const int minKeyNum = minimumDegree - 1;
    static const int maxKeyNum = 2 * minimumDegree - 1;
    static const int minChildNum = minimumDegree;
    static const int maxChildNum = 2 * minimumDegree;
    static const int minLeafSlot = minKeyNum;
    static const int maxLeafSlot = maxKeyNum;
    class Node
    {
    public:
        bool isLeaf;
        int keyNum;

        Node() { keyNum = 0;}

        virtual int getLower(int k) = 0;
    };

    class innerNode : public Node
    {
        friend class BPlusTree;
    private:
        Node* child[maxChildNum];
        long key[maxKeyNum];
    public:
		innerNode() {isLeaf = false;}
        virtual int getLower(int k);
        void insert(long key, Node *p);
        int split(Node* newNode, long k);
    };

    class leafNode : public Node
    {
        friend class BPlusTree;
    private:
        void* value[maxLeafSlot];
        long key[maxLeafSlot];
        leafNode *left;
        leafNode *right;
    public:
        leafNode() {isLeaf = true; left = NULL; right = NULL;}
        virtual int getLower(int k);
        void insert(long key, void* value);
        int split(leafNode* newNode);
    };

private:
    Node *root;
    leafNode* leftHead;
    sem_t lock;

private:
    void* get(Node* node, long key);
    void insertNode(Node* node, long key, void* value, Node* p, stack<Node *>* parent);
    void put(long key, void* value, bool f);

public:
    BPlusTree() {
        root = NULL;
        leftHead = NULL;
        sem_init(&lock, 0, 1);
    }

    void* get(long key);
    void put(long key, void* value);
    map<long, void*> getrange(long key1, long key2);
};

#endif
