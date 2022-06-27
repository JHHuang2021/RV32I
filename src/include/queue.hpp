#ifndef QUEUE_HPP
#define QUEUE_HPP
#include <cstring>
#include <utility>
template <class T, int len = 10>
struct Queue {
    int front = 0, rear = 0;
    T node[len + 1];
    bool busy[len + 1];

    Queue() { memset(busy, 0, sizeof(busy)); }
    void Clear() {
        front = rear = 0;
        memset(busy, 0, sizeof(busy));
    }
    bool Empty() { return front == rear; }
    bool Full() { return front == GetNext(rear); }
    int GetNext(int pos) { return pos >= len ? 0 : pos + 1; }
    int Allocate() {
        rear = GetNext(rear);
        busy[rear] = true;
        return rear;
    }
    int Length() { return len; }
    bool Pop() {
        if (Empty()) return false;
        front = GetNext(front);
        busy[front] = false;
        return true;
    }
    T& GetFront() {
        if (Empty())
            throw "";
        else
            return node[GetNext(front)];
    }
    int GetFrontInd() { return GetNext(front); }
    T& operator[](int pos) { return node[pos]; }
};

#endif