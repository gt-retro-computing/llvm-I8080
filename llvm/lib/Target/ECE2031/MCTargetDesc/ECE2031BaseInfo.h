//
// Created by codetector on 9/3/19.
//

#ifndef LLVM_ECE2031BASEINFO_H
#define LLVM_ECE2031BASEINFO_H
#include "ECE2031MCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
    namespace ECE2031 {
    }
    static inline unsigned getECE2031RegisterNumbering(unsigned Reg) {
        switch (Reg) {
            case ECE2031::zero: return 0;
            case ECE2031::at: return 1;
            case ECE2031::v0: return 2;
            case ECE2031::a0: return 3;
            case ECE2031::a1: return 4;
            case ECE2031::a2: return 5;
            case ECE2031::t0: return 6;
            case ECE2031::t1: return 7;
            case ECE2031::t2: return 8;
            case ECE2031::s0: return 9;
            case ECE2031::s1: return 10;
            case ECE2031::s2: return 11;
            case ECE2031::k0: return 12;
            case ECE2031::sp: return 13;
            case ECE2031::fp: return 14;
            case ECE2031::ra: return 15;
            default: llvm_unreachable("LMAO, What register is this????");
        }
    }
}

#endif //LLVM_ECE2031BASEINFO_H
