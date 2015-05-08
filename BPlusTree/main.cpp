#include "BPlusTree.hpp"
#include <string>
int main()
{
    BPlusTree bt;
    
    bt.put(26, 123);
    bt.put(29, 642);
    long r;
    map<long, long> res = bt.getrange(1,36);
    map<long, long>::iterator it;
    for (it = res.begin(); it != res.end(); ++it)
        std::cout << it->first << " => " << it->second << '\n';
    if(bt.get(16, r)) {
        cout << r << endl;
    }
}
