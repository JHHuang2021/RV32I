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
    int addr;
    int r;
    bool ready;
};
struct Reservation {
    Operation::Oper op;
    int vj, vk, qj, qk;
    int dest;
    int imm;
    int shamt;
    int step=1;//for load
};
struct RegisterStatus {
    int reorder;
    bool busy;
};
class Tomasulo {
   private:
    Queue<ReorderBufferItem, 20> rob;
    Array<Reservation, 20> rs;
    RegisterStatus reg_status[32];
    Memory memory;
    void Issue(const Operation &op);
    std::pair<bool, int> Execute(int r);
    void WriteResult(int r,int result);
    void Commit();

   public:
    void Run();
};
#endif