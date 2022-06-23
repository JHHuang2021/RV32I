#include "include/memory.h"

#include <cstring>
#include <iostream>

int Memory::CharToInt(char ch) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else
        return ch - 'A' + 10;
}

void Memory::Read() {
    char str[20];
    std::cin >> str;
    while (!std::cin.eof()) {
        if (str[0] == '@') {
            now_pc = 0;
            int len = strlen(str);
            for (int i = 1; i < len; i++)
                now_pc = now_pc * 16 + CharToInt(str[i]);
        } else
            mem[now_pc++] = CharToInt(str[0]) * 16 + CharToInt(str[1]);
        std::cin >> str;
    }
}

int Memory::Return() { return reg[10]; }

void Memory::ResetPC() { now_pc = 0; }

unsigned Memory::GetInstruction() {
    unsigned ret = 0;
    for (int i = now_pc + 3; i >= now_pc; i--)
        ret = ret << 8 | static_cast<unsigned>(mem[i]);
    now_pc += 4;
    return ret;
}

int Memory::Addi(int arg1, int imm) {
    return arg1 + imm;
    // now_pc += 4;
}

int Memory::Slti(int arg1, int imm) {
    return arg1 < imm;
    // now_pc += 4;
}

int Memory::Sltiu(int arg1, int imm) {
    return static_cast<unsigned>(arg1) < static_cast<unsigned>(imm);
    // now_pc += 4;
}

int Memory::Andi(int arg1, int imm) {
    return arg1 & imm;
    // now_pc += 4;
}

int Memory::Ori(int arg1, int imm) {
    return arg1 | imm;
    // now_pc += 4;
}

int Memory::Xori(int arg1, int imm) {
    return arg1 ^ imm;
    // now_pc += 4;
}

int Memory::Slli(int arg1, int shamt) {
    return arg1 << shamt;
    // now_pc += 4;
}

int Memory::Srli(int arg1, int shamt) {
    return static_cast<unsigned>(arg1) >> shamt;
    // now_pc += 4;
}

int Memory::Srai(int arg1, int shamt) {
    //算数位移
    return arg1 >> shamt;
    // now_pc += 4;
}

int Memory::Lui(int imm) {
    return imm;
    // now_pc += 4;
}

int Memory::Auipc(int imm) {
    return now_pc + imm - 4;
    // now_pc += 4;
}

int Memory::Add(int arg1, int arg2) {
    return arg1 + arg2;
    // now_pc += 4;
}

int Memory::Slt(int arg1, int arg2) {
    return arg1 < arg2;
    // now_pc += 4;
}

int Memory::Sltu(int arg1, int arg2) {
    return static_cast<unsigned>(arg1) < static_cast<unsigned>(arg2);
    // now_pc += 4;
}

int Memory::And(int arg1, int arg2) {
    return arg1 & arg2;
    // now_pc += 4;
}

int Memory::Or(int arg1, int arg2) {
    return arg1 | arg2;
    // now_pc += 4;
}

int Memory::Xor(int arg1, int arg2) {
    return arg1 ^ arg2;
    // now_pc += 4;
}

int Memory::Sll(int arg1, int arg2) {
    return arg1 << arg2;
    // now_pc += 4;
}

int Memory::Srl(int arg1, int arg2) {
    return static_cast<unsigned>(arg1) >> arg2;
    // now_pc += 4;
}

int Memory::Sra(int arg1, int arg2) {
    return arg1 >> arg2;
    // now_pc += 4;
}

int Memory::Jal(int imm) {
    now_pc += imm - 4;
    return now_pc + 4 - imm;
}

int Memory::Jalr(int arg1, int imm) {
    //
    int tmp = now_pc;
    now_pc = (arg1 + imm) & ~1;
    return tmp;
}

int Memory::Beq(int arg1, int arg2, int imm) {
    if (arg1 == arg2) {
        now_pc += imm - 4;
        return 1;
    } else
        return 0;
}

int Memory::Bne(int arg1, int arg2, int imm) {
    if (arg1 != arg2) {
        now_pc += imm - 4;
        return 1;
    } else
        return 0;
}

int Memory::Blt(int arg1, int arg2, int imm) {
    if (arg1 < arg2) {
        now_pc += imm - 4;
        return 1;
    } else
        return 0;
}

int Memory::Bge(int arg1, int arg2, int imm) {
    if (arg1 >= arg2) {
        now_pc += imm - 4;
        return 1;
    } else
        return 0;
}

int Memory::Bltu(int arg1, int arg2, int imm) {
    if (static_cast<unsigned>(arg1) < static_cast<unsigned>(arg2)) {
        now_pc += imm - 4;
        return 1;
    } else
        return 0;
}

int Memory::Bgeu(int arg1, int arg2, int imm) {
    if (static_cast<unsigned>(arg1) >= static_cast<unsigned>(arg2)) {
        now_pc += imm - 4;
        return 1;
    } else
        return 0;
}

int Memory::Lb(int imm) {
    if (mem[imm] >> 7)
        return (~0b11111111) | mem[imm];
    else
        return mem[imm];
}

int Memory::Lh(int imm) {
    unsigned read = 0;
    for (int i = 1; i >= 0; i--) read = (read << 8) | mem[imm + i];
    if (read >> 15)
        return (~0b1111111111111111) | read;
    else
        return read;
}

int Memory::Lw(int imm) {
    unsigned read = 0;
    for (int i = 3; i >= 0; i--) read = (read << 8) | mem[imm + i];
    return read;
}

int Memory::Lbu(int imm) { return mem[imm]; }

int Memory::Lhu(int imm) {
    unsigned read = 0;
    for (int i = 1; i >= 0; i--) read = (read << 8) | mem[imm + i];
    return read;
}

int Memory::Sb(int arg2, int imm) { mem[imm] = arg2; }

int Memory::Sh(int arg2, int imm) {
    unsigned tmp = arg2;
    for (int i = 0; i <= 1; i++) {
        mem[imm + i] = tmp;
        tmp >>= 8;
    }
}

int Memory::Sw(int arg2, int imm) {
    unsigned tmp = arg2;
    for (int i = 0; i <= 3; i++) {
        mem[imm + i] = tmp;
        tmp >>= 8;
    }
}

int Memory::Sub(int arg1, int arg2) { return arg1 - arg2; }

int Memory::Work(Operation::Oper op, int arg1, int arg2, int imm, int shamt) {
    switch (op) {
        case Operation::LUI:
            return Lui(imm);
        case Operation::AUIPC:
            return Auipc(imm);
        case Operation::JAL:
            return Jal(imm);
        case Operation::JALR:
            return Jalr(arg1, imm);
        case Operation::BEQ:
            return Beq(arg1, arg2, imm);
        case Operation::BNE:
            return Bne(arg1, arg2, imm);
        case Operation::BLT:
            return Blt(arg1, arg2, imm);
        case Operation::BGE:
            return Bge(arg1, arg2, imm);
        case Operation::BLTU:
            return Bltu(arg1, arg2, imm);
        case Operation::BGEU:
            return Bgeu(arg1, arg2, imm);
        case Operation::LB:
            return Lb(imm);
        case Operation::LH:
            return Lh(imm);
        case Operation::LW:
            return Lw(imm);
        case Operation::LBU:
            return Lbu(imm);
        case Operation::LHU:
            return Lhu(imm);
        case Operation::SB:
            return Sb(arg2, imm);
        case Operation::SH:
            return Sh(arg2, imm);
        case Operation::SW:
            return Sw(arg2, imm);
        case Operation::ADDI:
            return Addi(arg1, imm);
        case Operation::SLTI:
            return Slti(arg1, imm);
        case Operation::SLTIU:
            return Sltiu(arg1, imm);
        case Operation::XORI:
            return Xori(arg1, imm);
        case Operation::ORI:
            return Ori(arg1, imm);
        case Operation::ANDI:
            return Andi(arg1, imm);
        case Operation::SLLI:
            return Slli(arg1, shamt);
        case Operation::SRLI:
            return Srli(arg1, shamt);
        case Operation::SRAI:
            return Srai(arg1, shamt);
        case Operation::ADD:
            return Add(arg1, arg2);
        case Operation::SUB:
            return Sub(arg1, arg2);
        case Operation::SLL:
            return Sll(arg1, arg2);
        case Operation::SLT:
            return Slt(arg1, arg2);
        case Operation::SLTU:
            return Sltu(arg1, arg2);
        case Operation::XOR:
            return Xor(arg1, arg2);
        case Operation::SRL:
            return Srl(arg1, arg2);
        case Operation::SRA:
            return Sra(arg1, arg2);
        case Operation::OR:
            return Or(arg1, arg2);
        case Operation::AND:
            return And(arg1, arg2);
    }
    ResetRes();
}