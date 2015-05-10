#ifndef _BPLUSTREE_HPP
#define _BPLUSTREE_HPP

#include <iostream>
#include <stack>
#include <map>
#include <string>

#include "common.h"

using namespace std;

class BPlusTree
{
private:
    static const int minimumDegree = 7;

    static const int minKeyNum = minimumDegree - 1;
    static const int maxKeyNum = 2 * minimumDegree - 1;
    
    static const int minChildNum = minimumDegree;
    static const int maxChildNum = 2 * minimumDegree;
    
    static const int minLeafSlot = minKeyNum;
    static const int maxLeafSlot = maxKeyNum;
    class Node
    {
    public:
        int isLeaf;
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
		char padding[24];
    public:
		innerNode() {
			isLeaf = false;
			//std::cout << sizeof(*this) << std::endl;
			assert((sizeof(*this) % CACHELINE_SIZE) == 0);
		}
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
        leafNode() {
			isLeaf = true;
			left = NULL;
			right = NULL;
			//assert((sizeof(*this) % CACHELINE_SIZE) == 0);
		}
        virtual int getLower(int k);
        void insert(long key, void* value);
        int split(leafNode* newNode);
    };

private:
    Node *root;
    leafNode* leftHead;
    Lock *lock;
    char padding[40];

private:
    void* get(Node* node, long key);
    void insertNode(Node* node, long key, void* value, Node* p, stack<Node *>* parent);
    void putNode(long key, void* value);

public:
    BPlusTree() {
        root = NULL;
        leftHead = NULL;
		lock = new Lock();
		assert((sizeof(*this) % CACHELINE_SIZE) == 0);
    }

    void* get(long key, Stat* stat);
    void put(long key, void* value);
    map<long, void*> getrange(long key1, long key2);
};

#endif
