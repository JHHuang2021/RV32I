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
            pc = 0;
            int len = strlen(str);
            for (int i = 1; i < len; i++) pc = pc * 16 + CharToInt(str[i]);
        } else
            mem[pc++] = CharToInt(str[0]) * 16 + CharToInt(str[1]);
        std::cin >> str;
    }
    pc = 0;
}

unsigned Memory::GetInstr() {
    unsigned ret = 0;
    for (int i = pc + 3; i >= pc; i--)
        ret = ret << 8 | static_cast<unsigned>(cache.Read(i));
    nxt_pc = pc + 4;
    return ret;
}

int Memory::Load(Operation::Oper op, int addr) {
    unsigned read = 0;
    if (op == Operation::LB) {
        unsigned char tmp = cache.Read(addr);
        if (tmp >> 7)
            return (~0b11111111) | tmp;
        else
            return tmp;
    } else if (op == Operation::LH) {
        for (int i = 1; i >= 0; i--) read = (read << 8) | cache.Read(addr + i);
        if (read >> 15)
            return (~0b1111111111111111) | read;
        else
            return read;
    } else if (op == Operation::LW) {
        for (int i = 3; i >= 0; i--) read = (read << 8) | cache.Read(addr + i);
        return read;
    } else if (op == Operation::LBU)
        return cache.Read(addr);
    else if (op == Operation::LHU) {
        for (int i = 1; i >= 0; i--) read = (read << 8) | cache.Read(addr + i);
        return read;
    }
}

void Memory::Store(Operation::Oper op, int addr, int value) {
    if (op == Operation::SB)
        cache.Write(addr, value);
    else if (op == Operation::SH) {
        unsigned tmp = value;
        for (int i = 0; i <= 1; i++) {
            cache.Write(addr + i, tmp);
            tmp >>= 8;
        }
    } else if (op == Operation::SW) {
        unsigned tmp = value;
        for (int i = 0; i <= 3; i++) {
            cache.Write(addr + i, tmp);
            tmp >>= 8;
        }
    }
}