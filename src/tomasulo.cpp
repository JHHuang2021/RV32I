#include "include/tomasulo.h"
void Tomasulo::Issue(const Operation &op) {
    Reservation append;
    append.op = op.op;
    switch (op.op) {
        case Operation::LW:
        case Operation::LH:
        case Operation::LHU:
        case Operation::LB:
        case Operation::LBU:

            break;
        case Operation::SW:
        case Operation::SH:
        case Operation::SB:

            break;
        default:
            return;
    }
}

void Tomasulo::Execute() {}