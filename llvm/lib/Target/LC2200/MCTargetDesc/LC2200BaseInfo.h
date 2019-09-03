//
// Created by codetector on 9/3/19.
//

#ifndef LLVM_LC2200BASEINFO_H
#define LLVM_LC2200BASEINFO_H
#include "LC2200MCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
    namespace LC2200 {
    }
    static inline unsigned getLC2200RegisterNumbering(unsigned Reg) {
        switch (Reg) {
            case LC2200::zero: return 0;
            case LC2200::at: return 1;
            case LC2200::v0: return 2;
            case LC2200::a0: return 3;
            case LC2200::a1: return 4;
            case LC2200::a2: return 5;
            case LC2200::t0: return 6;
            case LC2200::t1: return 7;
            case LC2200::t2: return 8;
            case LC2200::s0: return 9;
            case LC2200::s1: return 10;
            case LC2200::s2: return 11;
            case LC2200::k0: return 12;
            case LC2200::sp: return 13;
            case LC2200::fp: return 14;
            case LC2200::ra: return 15;
            default: llvm_unreachable("LMAO, What register is this????");
        }
    }
}

#endif //LLVM_LC2200BASEINFO_H
