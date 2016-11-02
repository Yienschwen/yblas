#include <iostream>

#include "../matrix.hpp"

using namespace yblas::core;

int main() {
    mfloat mf1;
    mf1.resize(3,3);
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) {
            mf1.value(i, j) = (float) i / j;
        }
    }

    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) {
            std::cout << mf1.value(i, j) << '\t';
        }
        std::cout << '\n';
    }
    mfloat mf2 = mf1;
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) {
            std::cout << mf2.value(i, j) << '\t';
        }
        std::cout << '\n';
    }
    // std::size_t i, j;
    // std::cin >> i >> j;
    // std::cout << mf2.value(i, j) << '\n';

    mfloat mf3 = mf2;
    mf3.resize(4, 4);
    std::cout << "RESIZE 4, 4\n";
    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= 4; j++) {
            std::cout << mf3.value(i, j) << '\t';
        }
        std::cout << '\n';
    }
    return 0;
}