#include "fs.h"
#include <iostream>

using namespace std;

char fsMem[1 << 20];

int main() {
    SignalFS fs(fsMem, 1 << 20);
    fs.mkdir("home");
    fs.chdir("home");
    fs.mkdir("dir");
    fs.mkfile("text.txt", "Hello World!");
    vector<string> a = fs.list();
    for (string i : a) {
        cout << i << endl;
    }
    cout << endl;

    char b[15];
    fs.show("text.txt", b);
    cout << b << endl;
    cout << endl;

    fs.rm("text.txt", false);
    fs.chdir("..");

    a = fs.list("home");
    for (string i : a) {
        cout << i << endl;
    }
    return 0;
}
