#include "BPlusTree.hpp"
#include <string>
int main()
{
    BPlusTree bt;
    int* p1 = 32;
    int* p2 = 15;
    bt.put(26, p1);
    bt.put(29, p2);
    void* r;
    cout << bt.get(26, r) << endl;
    cout << bt.get(28, r) << endl;
}
