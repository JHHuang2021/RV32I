#ifndef TOMASULO_H
#define TOMASULO_H
#include <vector>

#include "memory.h"
#include "operation.h"
struct ReorderBufferItem {
    bool busy = true;
    Operation::Oper op;
    int dest;
    int value;
};
struct Reservation {
    bool busy = true;
    Operation::Oper op;
    int vj, vk, qj, qk;
    int dest;
};
struct RegisterStatus {
    int reorder;
    bool busy = false;
    // equvalent to qj==0
};
class Tomasulo {
   private:
    std::vector<ReorderBufferItem> reorder_buffer;
    std::vector<Reservation> reservation_station;
    RegisterStatus reg_status[32];
    Memory memory;
    void Issue(const Operation &op);
    void Execute(const Operation &op);
    void WriteResult(const Operation &op);

   public:
    int Run();
};
#endif