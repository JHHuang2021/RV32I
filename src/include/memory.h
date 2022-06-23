#ifndef MEMORY_H
#define MEMORY_H
#include "operation.h"
struct Memory {
    int reg[32] = {0};
    unsigned char mem[500000];
    unsigned tmp[3];
    int now_pc;

    int Return();
    void ResetRes() { reg[0] = 0; }
    void Read();
    int CharToInt(char ch);
    unsigned GetInstruction();
    void ResetPC();
    int Addi(int arg1, int imm);
    int Slti(int arg1, int imm);
    int Sltiu(int arg1, int imm);
    int Andi(int arg1, int imm);
    int Ori(int arg1, int imm);
    int Xori(int arg1, int imm);
    int Slli(int arg1, int shamt);
    int Srli(int arg1, int shamt);
    int Srai(int arg1, int shamt);
    int Lui(int imm);
    int Auipc(int imm);
    int Add(int arg1, int arg2);
    int Slt(int arg1, int arg2);
    int Sltu(int arg1, int arg2);
    int And(int arg1, int arg2);
    int Or(int arg1, int arg2);
    int Xor(int arg1, int arg2);
    int Sll(int arg1, int arg2);
    int Srl(int arg1, int arg2);
    int Sub(int arg1, int arg2);
    int Sra(int arg1, int arg2);
    int Jal(int imm);
    int Jalr(int arg1, int imm);
    int Beq(int arg1, int arg2, int imm);
    int Bne(int arg1, int arg2, int imm);
    int Blt(int arg1, int arg2, int imm);
    int Bge(int arg1, int arg2, int imm);
    int Bltu(int arg1, int arg2, int imm);
    int Bgeu(int arg1, int arg2, int imm);
    int Lw(int imm);
    int Lh(int imm);
    int Lhu(int imm);
    int Lb(int imm);
    int Lbu(int imm);
    int Sw(int arg2, int imm);
    int Sh(int arg2, int imm);
    int Sb(int arg2, int imm);
    int Work(Operation::Oper op, int arg1, int arg2, int imm, int shamt);
};
#endif