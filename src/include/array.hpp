#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <utility>
template <class T, int length = 10>
struct Array {
    T node[length];
    bool busy[length];
    int size = 0;

    Array() { memset(busy, 0, sizeof(busy)); }

    void Clear() {
        size = 0;
        memset(busy, 0, sizeof(busy));
    }

    bool Empty() { return size == 0; }

    bool Full() { return size == length; }

    std::pair<bool, int> Push(T newnode) {
        if (Full()) return std::make_pair(false, 0);

        for (int i = 0; i < length; i++)
            if (!busy[i]) {
                busy[i] = 1;
                node[i] = newnode;
                size++;
                return std::make_pair(true, i);
            }
    }

    bool Pop(int pos) {
        if (Empty()) return false;

        busy[pos] = 0;
        size--;
        return true;
    }
};

#endif