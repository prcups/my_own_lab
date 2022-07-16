#include "fs.h"
#include <iostream>

using namespace std;

char fsMem[1 << 20];

int main() {
    SignalFS fs(fsMem, 1 << 20);
    fs.mkdir("home");
    fs.mkdir("etc");
    vector<string> a = fs.list();
    for (string i : a) {
        cout << i << endl;
    }
    return 0;
}
