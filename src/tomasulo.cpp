#include "include/tomasulo.h"

#include <bits/stdc++.h>

#include "include/parser.h"
void Tomasulo::Issue(const Operation &op) {
    if (!rob.Full() && !rs.Full()) {
        int r = rs.GetIndex(), b = rob.GetIndex();
        rs.ModifyBusy(r, true);
        rs[r].dest = b;
        rob[b].r = r;
        rob[b].op = op.op;
        rob[b].dest = op.rd;
        rob[b].ready = false;
        if (op.rs1 != -1) {
            if (reg_status[op.rs1].busy) {
                int h = reg_status[op.rs1].reorder;
                if (rob[h].ready) {
                    rs[r].vj = rob[h].value;
                    rs[r].qj = 0;
                } else
                    rs[r].qj = h;
            } else {
                rs[r].vj = memory.reg[op.rs1];
                rs[r].qj = 0;
            }
        }
        if (op.rs2 != -1) {
            if (reg_status[op.rs2].busy) {
                int h = reg_status[op.rs2].reorder;
                if (rob[h].ready) {
                    rs[r].vk = rob[h].value;
                    rs[r].qk = 0;
                } else
                    rs[r].qk = h;
            } else {
                rs[r].vk = memory.reg[op.rs2];
                rs[r].qk = 0;
            }
        }
        rs[r].imm = op.imm;
        rs[r].shamt = op.shamt;
        switch (op.op) {
            case Operation::LW:
            case Operation::LH:
            case Operation::LHU:
            case Operation::LB:
            case Operation::LBU:
                reg_status[op.rs2].reorder = b;
                reg_status[op.rs2].busy = true;
                rob[b].dest = op.rs2;
                break;
            default:
                return;
        }
    }
}

std::pair<bool, int> Tomasulo::Execute(int r) {
    auto op = rs[r].op;
    switch (op) {
        case Operation::LW:
        case Operation::LH:
        case Operation::LHU:
        case Operation::LB:
        case Operation::LBU:
            if (rs[r].step == 1) {
                // step 1
                for (int i = rob.GetNext(rob.front); i != rob.GetNext(rob.rear);
                     i = rob.GetNext(i))
                    if (rob[i].op == Operation::SW ||
                        rob[i].op == Operation::SH ||
                        rob[i].op == Operation::SB)
                        return std::make_pair(false, 0);
                if (rs[r].qj != 0) return std::make_pair(false, 0);
                rs[r].imm += rs[r].vj;
                rs[r].step++;
                return std::make_pair(true, 0);
            } else {
                // step 2
                for (int i = rob.GetNext(rob.front); i != rob.GetNext(rob.rear);
                     i = rob.GetNext(i))
                    if (rob[i].op == Operation::SW ||
                        rob[i].op == Operation::SH ||
                        rob[i].op == Operation::SB)
                        if (rob[i].addr == rs[r].imm)
                            return std::make_pair(false, 0);
                return std::make_pair(
                    true, memory.Work(op, rs[r].vj, rs[r].vk, rs[r].imm,
                                      rs[r].shamt));
            }
        case Operation::SW:
        case Operation::SH:
        case Operation::SB:
            if (rs[r].qj == 0 && rs[r].dest == rob.GetNext(rob.front)) {
                rob.GetFront().second.addr = rs[r].vj + rs[r].imm;
                return std::make_pair(true, 0);
            }
            return std::make_pair(false, 0);
        default:
            if (rs[r].qj == 0 && rs[r].qk == 0)
                return std::make_pair(
                    true, memory.Work(op, rs[r].vj, rs[r].vk, rs[r].imm,
                                      rs[r].shamt));
    }
}

void Tomasulo::WriteResult(int r, int result) {
    int b = rs[r].dest;
    Operation::Oper op = rob[b].op;
    if (op == Operation::SW || op == Operation::SH || op == Operation::SB) {
        if (rs[r].qk == 0) rob.GetFront().second.value = rs[r].vk;
    } else {
        rs.ModifyBusy(r, false);
        for (int i = 0; i < rs.Length(); i++) {
            if (rs[i].qj == b) {
                rs[i].vj = result;
                rs[i].qj = 0;
            }
            if (rs[i].qk == b) {
                rs[i].vk = result;
                rs[i].qk = 0;
            }
        }
        rob[b].value = result;
        rob[b].ready = true;
    }
}

void Tomasulo::Commit() {
    auto rob_entry = rob.GetFront().second;
    int h = rob.GetNext(rob.front);
    if (rob_entry.ready) {
        int d = rob_entry.dest;
        if (rob_entry.op == Operation::BEQ || rob_entry.op == Operation::BNE ||
            rob_entry.op == Operation::BGE || rob_entry.op == Operation::BGEU ||
            rob_entry.op == Operation::BLT || rob_entry.op == Operation::BLTU) {
            if (!rob_entry.value) {
                rob.Clear();
                memset(reg_status, 0, sizeof(reg_status));
            }
        } else if (rob_entry.op == Operation::SW ||
                   rob_entry.op == Operation::SH ||
                   rob_entry.op == Operation::SB)
            memory.Work(rob_entry.op, 0, rob_entry.value, rob_entry.addr, 0);
        else
            memory.reg[d] = rob_entry.value;
        rob.Pop();
        if (reg_status[d].reorder == h) reg_status[d].busy = false;
    }
}

void Tomasulo::Run() {
    int cnt = 0;
    memory.Read();
    memory.ResetPC();
    while (true) {
        cnt++;
        unsigned code = memory.GetInstruction();
        if (code == 0x0ff00513) {
            std::cout << static_cast<unsigned>(memory.Return() & 255u)
                      << std::endl;
            return;
        }
        Operation op = Parse(code);
        Issue(op);
        auto ret = Execute(rob.GetFront().second.r);
        if (ret.first) WriteResult(rob.GetFront().second.r, ret.second);
        Commit();
    }
}