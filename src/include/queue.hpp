#ifndef QUEUE_HPP
#define QUEUE_HPP
#include <utility>
template <class T, int length = 10>
class Queue {
    int front = 0, rear = 0;
    T node[length + 1];
    bool busy[length + 1];

    Queue() { memset(busy, 0, sizeof(busy)); }

    void Clear() {
        front = rear = 0;
        memset(busy, 0, sizeof(busy));
    }

    int GetNext(int now) { return (now + 1) >= (length + 1) ? 0 : now + 1; }

    int GetPre(int now) { return (now - 1) < 0 ? length : now - 1; }

    bool Empty() { return front == rear; }

    bool Full() { return GetNext(rear) == front; }

    std::pair<bool, int> Push(const T& newnode) {
        if (Full()) return std::make_pair(false, 0);

        node[rear = GetNext(rear)] = newnode;
        node[rear].pos = rear;
        busy[rear] = 1;
        return std::make_pair(true, rear);
    }

    bool Pop() {
        if (Empty()) return false;
        front = GetNext(front);
        busy[front] = 0;
    }

    bool PopRear() {
        if (Empty()) return false;
        busy[rear] = 0;
        rear = GetPre(rear);
    }

    std::pair<bool, T&> GetRear() {
        if (Empty())
            return std::make_pair(false, node[0]);
        else
            return std::make_pair(true, node[rear]);
    }

    std::pair<bool, T&> GetFront() {
        if (Empty())
            return std::make_pair(false, node[0]);
        else
            return std::make_pair(true, node[GetNext(front)]);
    }

    std::pair<bool, T&> GetSec() {
        if (Empty() || GetNext(front) == rear)
            return std::make_pair(false, node[0]);
        else
            return std::make_pair(true, node[GetNext(GetNext(front))]);
    }

    int NextPos() { return GetNext(rear); }
};
#endif