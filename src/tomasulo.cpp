#include "include/tomasulo.h"

#include <iostream>

#include "include/parser.h"
bool Tomasulo::Issue() {
    unsigned code = memory.GetInstr();
    auto instr = Parse(code);
    if (rob.Full()) {
        memory.pc -= 4;
        return false;
    }
    if (instr.rd != -1 && reg_stat[instr.rd].busy) {
        memory.pc -= 4;
        return false;
    }

    int b = rob.Allocate();
    rob[b].Clear();

    rob[b].op = instr.op;
    rob[b].dest = instr.rd;
    rob[b].ready = false;
    rob[b].code = code;

    if (instr.op >= 11 && instr.op <= 18) {
        // load or store
        if (ls.Full()) return false;
        int s = ls.Allocate();
        ls[s].Clear();
        ls[s].op = instr.op;
        ls[s].dest = b;
        ls[s].imm = instr.imm;
        if (instr.rs1 != -1) {
            if (reg_stat[instr.rs1].busy) {
                int h = reg_stat[instr.rs1].reorder;
                if (rob[h].ready) {
                    ls[s].vj = rob[h].value;
                    ls[s].qj = -1;
                } else
                    ls[s].qj = h;
            } else {
                ls[s].vj = inreg[instr.rs1];
                ls[s].qj = -1;
            }
        }
        if (instr.rs2 != -1) {
            if (reg_stat[instr.rs2].busy) {
                int h = reg_stat[instr.rs2].reorder;
                if (rob[h].ready) {
                    ls[s].vk = rob[h].value;
                    ls[s].qk = -1;
                } else
                    ls[s].qk = h;
            } else {
                ls[s].vk = inreg[instr.rs2];
                ls[s].qk = -1;
            }
        }
    } else {
        if (rs.Full()) return false;
        rob[b].addr = memory.pc;
        rob[b].offset = instr.imm;
        int r = rs.Allocate();
        rs[r].Clear();
        rs[r].op = instr.op;
        rs[r].dest = b;
        rs[r].imm = instr.imm;
        rs[r].shamt = instr.shamt;
        rs[r].cur_pc = memory.pc;

        if (instr.rs1 != -1) {
            if (reg_stat[instr.rs1].busy) {
                int h = reg_stat[instr.rs1].reorder;
                if (rob[h].ready) {
                    rs[r].vj = rob[h].value;
                    rs[r].qj = -1;
                } else
                    rs[r].qj = h;
            } else {
                rs[r].vj = inreg[instr.rs1];
                rs[r].qj = -1;
            }
        }
        if (instr.rs2 != -1) {
            if (reg_stat[instr.rs2].busy) {
                int h = reg_stat[instr.rs2].reorder;
                if (rob[h].ready) {
                    rs[r].vk = rob[h].value;
                    rs[r].qk = -1;
                } else
                    rs[r].qk = h;
            } else {
                rs[r].vk = inreg[instr.rs2];
                rs[r].qk = -1;
            }
        }
    }
    if (instr.rd != -1) {
        reg_stat[instr.rd].reorder = b;
        reg_stat[instr.rd].busy = true;
    }
    return true;
}

void Tomasulo::Execute() {
    // int r;
    // for (r = 0; r < rs.Length(); r++)
    //     if (rs.busy[r] && rs[r].qj == -1 && rs[r].qk == -1) break;
    // if (r == rs.Length()) return;
    for (int i = 0; i < now.ex_size; i++) {
        RSLItem &it = rs[now.ex[i]];
        int b = it.dest;
        switch (it.op) {
            case Operation::JAL:
                rob[b].value = it.cur_pc;
                break;
            case Operation::JALR:
                rob[b].value = it.cur_pc;
                rob[b].pc = (it.vj + it.imm) & ~1;
                break;
            case Operation::LUI:
                rob[b].value = it.imm;
                break;
            case Operation::AUIPC:
                rob[b].value = it.imm + it.cur_pc - 4;
                break;
            default:
                rob[b].value = alu.Work(it.op, it.vj, it.vk, it.imm, it.shamt);
        }
        rs.Erase(now.ex[i]);

        // update
        // for (int i = 0; i < rs.Length(); i++) {
        //     if (rs[i].qj == b) {
        //         rs[i].vj = ret;
        //         rs[i].qj = -1;
        //     }
        //     if (rs[i].qk == b) {
        //         rs[i].vk = ret;
        //         rs[i].qk = -1;
        //     }
        // }
        // for (int i = 0; i < ls.Length(); i++) {
        //     if (ls[i].qj == b) {
        //         ls[i].vj = ret;
        //         ls[i].qj = -1;
        //     }
        //     if (ls[i].qk == b) {
        //         ls[i].vk = ret;
        //         ls[i].qk = -1;
        //     }
        // }
        rob[b].ready = true;
    }
}

bool Tomasulo::SLBuffer() {
    if (ls.Empty()) return false;
    RSLItem &it = ls.GetFront();
    if (it.qj != -1 || it.qk != -1) return false;
    if (it.cnt != 3) {
        it.cnt++;
        return false;
    }
    if (it.op >= 11 && it.op <= 15) {
        // load
        int b = it.dest;
        rob[b].addr = it.vj + it.imm;
        rob[b].value = memory.Load(it.op, rob[b].addr);
        rob[b].ready = true;
        ls.Pop();
        now.ls = b;
        now.ls_if_update = true;
        // for (int i = 0; i < rs.Length(); i++) {
        //     if (rs[i].qj == b) {
        //         rs[i].vj = rob[b].value;
        //         rs[i].qj = -1;
        //     }
        //     if (rs[i].qk == b) {
        //         rs[i].vk = rob[b].value;
        //         rs[i].qk = -1;
        //     }
        // }
        // for (int i = 0; i < ls.Length(); i++) {
        //     if (ls[i].qj == b) {
        //         ls[i].vj = rob[b].value;
        //         ls[i].qj = -1;
        //     }
        //     if (ls[i].qk == b) {
        //         ls[i].vk = rob[b].value;
        //         ls[i].qk = -1;
        //     }
        // }
        return true;
    } else {
        // store
        if (rob.GetFrontInd() == it.dest) {
            int b = it.dest;
            rob[b].addr = it.vj + it.imm;

            memory.Store(it.op, rob[b].addr, it.vk);
            rob[b].ready = true;
            ls.Pop();
            return true;
        } else
            return false;
    }
}

void Tomasulo::Commit() {
    if (rob.Empty()) return;
    RobItem &it = rob.GetFront();
    int h = rob.GetFrontInd();
    if (it.code == 0x0ff00513) {
        std::cout << static_cast<unsigned>(outreg[10] & 255u) << std::endl;
        exit(0);
    }
    if (it.ready) {
        int d = it.dest;
        if (it.op >= 5 && it.op <= 10) {
            // branch
            if (it.value) {
                rob.Clear();
                rs.Clear();
                ls.Clear();
                memset(reg_stat, 0, sizeof(reg_stat));
                memory.pc = it.addr + it.offset - 4;
            }
        } else if (it.op >= 3 && it.op <= 4) {
            outreg[it.dest] = it.value;
            rob.Clear();
            rs.Clear();
            ls.Clear();
            memset(reg_stat, 0, sizeof(reg_stat));
            if (it.op == 3)
                memory.pc = it.addr + it.offset - 4;
            else
                memory.pc = it.pc;
        } else
            outreg[it.dest] = it.value;
        rob.Pop();
        if (reg_stat[it.dest].reorder == h) reg_stat[it.dest].busy = false;
    }
    return;
}

void Tomasulo::Update() {
    for (int i = 0; i < 32; i++) inreg[i] = outreg[i];

    for (int j = 0; j < now.ex_size; j++) {
        int b = rs[now.ex[j]].dest;
        for (int i = 0; i < rs.Length(); i++) {
            if (rs[i].qj == b) {
                rs[i].vj = rob[b].value;
                rs[i].qj = -1;
            }
            if (rs[i].qk == b) {
                rs[i].vk = rob[b].value;
                rs[i].qk = -1;
            }
        }
        for (int i = 0; i <= ls.Length(); i++) {
            if (ls[i].qj == b) {
                ls[i].vj = rob[b].value;
                ls[i].qj = -1;
            }
            if (ls[i].qk == b) {
                ls[i].vk = rob[b].value;
                ls[i].qk = -1;
            }
        }
    }
    if (now.ls_if_update) {
        int b = now.ls;
        for (int i = 0; i < rs.Length(); i++) {
            if (rs[i].qj == b) {
                rs[i].vj = rob[b].value;
                rs[i].qj = -1;
            }
            if (rs[i].qk == b) {
                rs[i].vk = rob[b].value;
                rs[i].qk = -1;
            }
        }
        for (int i = 0; i <= ls.Length(); i++) {
            if (ls[i].qj == b) {
                ls[i].vj = rob[b].value;
                ls[i].qj = -1;
            }
            if (ls[i].qk == b) {
                ls[i].vk = rob[b].value;
                ls[i].qk = -1;
            }
        }
    }
    now.ls_if_update = false;
    for (int i = 0; i < now.nxt_ex_size; i++) now.ex[i] = now.nxt_ex[i];
    now.ex_size = now.nxt_ex_size;
}

void Tomasulo::Reservation() {
    now.nxt_ex_size = 0;
    for (int r = 0; r < rs.Length() && now.nxt_ex_size < ALU; r++)
        if (rs.busy[r] && rs[r].qj == -1 && rs[r].qk == -1)
            now.nxt_ex[now.nxt_ex_size++] = r;
}

void Tomasulo::Run() {
    memory.Read();
    ls.Clear();
    rs.Clear();
    rob.Clear();
    memset(reg_stat, 0, sizeof(reg_stat));
    while (true) {
        static int cnt = 0;
        cnt++;
        Issue();
        ResetRes();

        Reservation();
        ResetRes();

        Execute();
        ResetRes();

        SLBuffer();
        ResetRes();

        Commit();
        ResetRes();

        Update();
        ResetRes();
    }
}