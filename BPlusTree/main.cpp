#include "BPlusTree.hpp"
#include <string>

int main()
{
    BPlusTree bt;

    long a = 123, b = 642;

    cout << &a << endl;

    bt.put(26, &a);
    bt.put(29, &b);

    
    cout << *(long*)bt.get(26) << endl;
    //cout << *(long*)bt.get(33) << endl;
    
}
