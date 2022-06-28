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
    if (exr == rs.Length()) return;
    RSLItem &it = rs[exr];
    exb = it.dest;
    switch (it.op) {
        case Operation::JAL:
            rob[exb].value = it.cur_pc;
            break;
        case Operation::JALR:
            rob[exb].value = it.cur_pc;
            rob[exb].pc = (it.vj + it.imm) & ~1;
            break;
        case Operation::LUI:
            rob[exb].value = it.imm;
            break;
        case Operation::AUIPC:
            rob[exb].value = it.imm + it.cur_pc - 4;
        default:
            rob[exb].value = alu.Work(it.op, it.vj, it.vk, it.imm, it.shamt);
    }
    rs.Erase(exr);
    rob[exb].ready = true;
}

void Tomasulo::Reservation() {
    int r = rs.Length();
    for (r = 0; r < rs.Length(); r++)
        if (rs.busy[r] && rs[r].qj == -1 && rs[r].qk == -1) break;
    exr = r;
}

bool Tomasulo::SLBuffer() {
    if (ls.Empty()) return false;
    RSLItem &it = ls.GetFront();
    if (it.qj != -1 || it.qk != -1) return false;
    if (it.cnt != 3) {
        it.cnt++;
        exs = 0;
        return false;
    }
    if (it.op >= 11 && it.op <= 15) {
        // load
        int b = it.dest;
        rob[b].addr = it.vj + it.imm;
        rob[b].value = memory.Load(it.op, rob[b].addr);
        rob[b].ready = true;
        ls.Pop();
        exs = 1;
        exbs = b;
        return true;
    } else {
        // store
        exs = 0;
        if (rob.GetFrontInd() == it.dest) {
            int b = it.dest;
            rob[b].addr = it.vj + it.imm;
            rob[b].value = it.vk;
            memory.Store(it.op, rob[b].addr, rob[b].value);
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
                memset(reg_stat, 0, sizeof(reg_stat));
                memory.pc = it.addr + it.offset - 4;
            }
        } else if (it.op >= 3 && it.op <= 4) {
            outreg[it.dest] = it.value;
            rob.Clear();
            rs.Clear();
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
    if (exr != rs.Length()) {
        // update
        for (int i = 0; i < rs.Length(); i++) {
            if (rs[i].qj == exb) {
                rs[i].vj = rob[exb].value;
                rs[i].qj = -1;
            }
            if (rs[i].qk == exb) {
                rs[i].vk = rob[exb].value;
                rs[i].qk = -1;
            }
        }
        for (int i = 0; i < ls.Length(); i++) {
            if (ls[i].qj == exb) {
                ls[i].vj = rob[exb].value;
                ls[i].qj = -1;
            }
            if (ls[i].qk == exb) {
                ls[i].vk = rob[exb].value;
                ls[i].qk = -1;
            }
        }
    }
    if (exs) {
        for (int i = 0; i < rs.Length(); i++) {
            if (rs[i].qj == exbs) {
                rs[i].vj = rob[exbs].value;
                rs[i].qj = -1;
            }
            if (rs[i].qk == exbs) {
                rs[i].vk = rob[exbs].value;
                rs[i].qk = -1;
            }
        }
        for (int i = 0; i < ls.Length(); i++) {
            if (ls[i].qj == exbs) {
                ls[i].vj = rob[exbs].value;
                ls[i].qj = -1;
            }
            if (ls[i].qk == exbs) {
                ls[i].vk = rob[exbs].value;
                ls[i].qk = -1;
            }
        }
    }
}

void Tomasulo::Run() {
    int cnt = 0;
    memory.Read();
    while (true) {
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