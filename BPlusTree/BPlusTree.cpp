#include "BPlusTree.hpp"

int BPlusTree::innerNode::getLower(int k)
{
    if (keyNum == 0) return 0;
    int lo = 0, hi = keyNum;

    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (key[mid] > k) hi = mid;
        else lo = mid + 1;
    }

    return hi;
}

void BPlusTree::innerNode::insert(long k, Node *p)
{
    int i = 0;
    for (i = keyNum-1; i >= 0 && key[i] > k; i--) {
        key[i+1] = key[i];
        child[i+2] = child[i+1];
    }
    key[i+1] = k;
    child[i+2] = p;
    keyNum++;
}

int BPlusTree::innerNode::split(Node*  newNode, long k)
{
    if (k > key[minKeyNum] && k < key[minKeyNum+1]) {
        for (int i = 0; i < minKeyNum; i++) {
            ((innerNode *)newNode)->key[i] = key[i+minKeyNum+1];
            ((innerNode *)newNode)->child[i+1] = child[i+minKeyNum+2];
        }
        newNode->keyNum = minKeyNum;
        keyNum = minKeyNum + 1;
        return k;
    }
    else {
        long retkey = 0;
        if (k > key[minKeyNum + 1]) {
            retkey = key[minKeyNum + 1];
            for (int i = 0; i < minKeyNum - 1; i++) {
                ((innerNode *)newNode)->key[i] = key[i + minKeyNum + 2];
                ((innerNode *)newNode)->child[i+1] = child[i+minKeyNum+3];
            }
            ((innerNode *)newNode)->child[0] = child[minKeyNum + 2];
            newNode->keyNum = minKeyNum - 1;
            keyNum = minKeyNum + 1;
            return retkey;
        }
        else {
            retkey = key[minKeyNum];
            for (int i = 0; i < minKeyNum; i++) {
                ((innerNode *)newNode)->key[i] = key[i + minKeyNum + 1];
                ((innerNode *)newNode)->child[i+1] = child[i+minKeyNum+2];
            }
            ((innerNode *)newNode)->child[0] = child[minKeyNum + 1];
            newNode->keyNum = minKeyNum;
            keyNum = minKeyNum;
            return retkey;
        }
    }
}

//<P0 K0 P1 K1 ... Pn Kn Pn+1>
//for  Ki <= key < Ki+1 ,key store in subtree pointed by Pi+1
//find first Ki that is greater than key, return i;
int BPlusTree::leafNode::getLower(int k)
{
    if (keyNum == 0) return 0;
    int lo = 0, hi = keyNum;

    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (key[mid] > k) hi = mid;
        else lo = mid + 1;
    }

    return hi;
}

void BPlusTree::leafNode::insert(long k, long v)
{
    int i = 0;
    for (i = keyNum-1; i >= 0 && key[i] > k; i--) {
        key[i+1] = key[i];
        value[i+1] = value[i];
    }
    key[i+1] = k;
    value[i+1] = v;
    keyNum++;
}

int BPlusTree::leafNode::split(leafNode* newNode)
{
    for (int i = 0; i < minKeyNum; i++) {
        newNode->key[i] = key[i+minKeyNum+1];
        newNode->value[i] = value[i + minKeyNum+1];
    }
    keyNum = minKeyNum + 1;
    newNode->keyNum = minKeyNum;

    newNode->right = right;
    newNode->left = this;
    if (right != NULL) {
        right->left = newNode;
    }
    right = newNode;

    return newNode->key[0];
}


bool BPlusTree::get(Node* node, long key, long & v)
{
    if (node == NULL) return false;

    if (node->isLeaf) {
        int slot = node->getLower(key) - 1;
        if (((leafNode*)node)->key[slot] == key) {
            v = ((leafNode*)node)->value[slot];
            return true;
        }
    }
    else {
        int slot = node->getLower(key);
        return get(((innerNode*)node)->child[slot], key, v);
    }

    return false;
}

bool BPlusTree::get(long key, long& result)
{
    bool b = false;
    RTM_EXEC(lock,
        b = get(root, key, result);
    )
    return b;
}

void BPlusTree::insertNode(Node* node, long key, long v, Node *p, stack<Node*>* parent)
{
    Node* newNode;
    long newKey;
    if (node->keyNum < maxKeyNum) {
        if (node->isLeaf) ((leafNode*)node)->insert(key, v);
        else ((innerNode *)node)->insert(key, p);
    }
    else {
        if (node->isLeaf) {
            newNode = new leafNode();
            newKey = ((leafNode*)node)->split((leafNode *)newNode);

            if (key < newKey) {
                ((leafNode*)node)->insert(key, v);
            }
            else
                ((leafNode*)newNode)->insert(key, v);
        }
        else {
            newNode = new innerNode();
            newKey = ((innerNode*)node)->split(newNode, key);
            if (newKey == key) {
                ((innerNode *)newNode)->child[0] = p;
            }
            else if (newKey > key) {
                ((innerNode*)node)->insert(key, p);
            }
            else {
                ((innerNode*)newNode)->insert(key, p);
            }
        }

        if (node == root) {
            innerNode* newRoot = new innerNode();
            newRoot->keyNum = 1;
            newRoot->key[0] = newKey;
            newRoot->child[0] = node;
            newRoot->child[1] = newNode;
            root = newRoot;
        }
        else {
            Node* p = parent->top();
            parent->pop();
            insertNode(p, newKey, v, newNode, parent);
        }
    }
}

bool BPlusTree::put(long key, long value, bool f)
{
    stack<Node *> parent;
    Node *n = root;
    if (root == NULL) {
        root = new leafNode();
        insertNode(root , key, value, NULL , &parent);
        return true;
    }

    while (n->isLeaf == false) {
        innerNode *inner = (innerNode*) n;
        int slot = inner->getLower(key);
        parent.push(n);
        n = inner->child[slot];
    }

    leafNode *leaf = (leafNode *)n;

    int slot = leaf->getLower(key);
    if (slot > 0) slot -= 1; 
    if (leaf->key[slot] == key) leaf->value[slot] = value;
    else insertNode(leaf , key, value, NULL , &parent);

    return true;
}

void BPlusTree::put(long key, long value)
{
    RTM_EXEC(lock,
        put(key, value, true);
    )
}

map<long, long> BPlusTree::getrange(long key1, long key2)
{
    Node *n = root;
    int i = 0;
    map<long, long> res;
    if (root == NULL) return res;
    while (n->isLeaf == false) {
        innerNode *inner = (innerNode*) n;
        int slot = inner->getLower(key1);
        n = inner->child[slot];
    }

    leafNode *leaf = (leafNode *)n;

    int slot = leaf->getLower(key1)-1;
	
    if (leaf->key[slot] != key1) slot++;

    i = slot;
    while (true) {
        for (; i < leaf->keyNum && leaf->key[i] >= key1 && leaf->key[i] <= key2; i++) {
            res[leaf->key[i]] = leaf->value[i];
        }
        if (i >= leaf->keyNum) {
            i = 0;
            leaf = leaf -> right;
        } 
        else break;
    }
    return res;
}
