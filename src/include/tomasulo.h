#ifndef TOMASULO_H
#define TOMASULO_H
#include <vector>

#include "alu.h"
#include "array.hpp"
#include "memory.h"
#include "operation.h"
#include "queue.hpp"
const int ALU = 4;
const int COUNTER = 100;

class Tomasulo {
   private:
    struct RobItem {
        Operation::Oper op;
        int dest;  // reg
        int addr;  // mem
        int pc;
        int value;
        bool ready = false;
        int offset;
        int prediction = 0;  // 0 false 1 true
        int counter = 0;
        unsigned code = 0;
        void Clear() {
            op = Operation::LUI;
            dest = value = addr = pc = -1;
            offset = 0;
            ready = false;
        }
    };
    struct RSLItem {
        Operation::Oper op;
        int vj, vk, qj, qk;
        int dest;
        int imm;
        int shamt;
        int cur_pc;  // for JAL and JALR
        int cnt;     // for Load and Store
        int status = 0;
        // 0 idle 1 pending 2 finished
        void Clear() {
            vj = vk = qj = qk = -1;
            dest = imm = shamt = -1;
            cnt = 1;
            status = cur_pc = 0;
        }
    };
    struct RegStatus {
        int reorder;
        bool busy = false;
    };
    struct Status {
        int ex_size = 0;
        int ex[ALU];
        int nxt_ex_size = 0;
        int nxt_ex[ALU];
        int ls;

        bool ls_if_update = false;

        void Clear() {
            ex_size = nxt_ex_size = 0;
            memset(ex, 0, sizeof(ex));
            memset(nxt_ex, 0, sizeof(nxt_ex));
        }
    };
    struct Counter {
        int b;
        int his;
        int counter[4];
        Counter() {
            // for (int i = 0; i <= 3; i++) counter[i] = 3;
        }
    };
    Memory memory;
    Alu alu;
    Queue<RobItem> rob;
    Queue<RSLItem> ls;
    Array<RSLItem> rs;
    Array<Counter, COUNTER> counter;
    RegStatus reg_stat[32];
    Status now;
    int inreg[32], outreg[32];
    bool Issue();
    void Execute();
    bool SLBuffer();
    void Reservation();
    void Commit();
    void Update();
    void ResetRes() { outreg[0] = 0; }

    int prediction = 0;
    int succ = 0;
    bool rollback = false;

   public:
    void Run();
};
#endif