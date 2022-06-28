#include <iostream>

#include "include/tomasulo.h"
Tomasulo CPU;
int main() {
    freopen("testcases/superloop.data", "r", stdin);
    freopen("test.out", "w", stdout);
    CPU.Run();
    return 0;
}