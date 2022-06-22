#ifndef TOMASULO_H
#define TOMASULO_H
#include <list>

#include "array.hpp"
#include "memory.h"
#include "operation.h"
#include "queue.hpp"
struct ReorderBufferItem {
    Operation::Oper op;
    int dest;
    int value;
};
struct Reservation {
    Operation::Oper op;
    int vj, vk, qj, qk;
    int dest;
    int imm;
};
struct RegisterStatus {
    int reorder;
    int q = 0;
};
class Tomasulo {
   private:
    Queue<ReorderBufferItem, 20> reorder_buffer;
    Array<Reservation, 20> reservation_station;
    RegisterStatus reg_status[32];
    Memory memory;
    void Issue(const Operation &op);
    void Execute();
    void WriteResult(const Operation &op);

   public:
    int Run();
};
#endif