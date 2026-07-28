#include <bits/stdc++.h>
using namespace std;
int main() {
    int n = 1000000;
    int m = 1000000;
    srand(time(0));
    ofstream out("input.txt");
    for (int i = 0; i < m; i++) {
        int u = rand() % n + 1;
        int v = rand() % n + 1;
        int w = rand() % 100 + 1;
        out << u << " " << v << " " << w << "\n";
    }
}