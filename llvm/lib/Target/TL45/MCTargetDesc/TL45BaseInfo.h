//
// Created by codetector on 9/3/19.
//

#ifndef LLVM_TL45BASEINFO_H
#define LLVM_TL45BASEINFO_H
#include "TL45MCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
    namespace TL45 {
    }
    static inline unsigned getTL45RegisterNumbering(unsigned Reg) {
        switch (Reg) {
            case TL45::r0: return 0;
            case TL45::r1: return 1;
            case TL45::r2: return 2;
            case TL45::r3: return 3;
            case TL45::r4: return 4;
            case TL45::r5: return 5;
            case TL45::r6: return 6;
            case TL45::r7: return 7;
            case TL45::r8: return 8;
            case TL45::r9: return 9;
            case TL45::r10: return 10;
            case TL45::r11: return 11;
            case TL45::r12: return 12;
            case TL45::r13: return 13;
            case TL45::bp: return 14;
            case TL45::sp: return 15;
            default: llvm_unreachable("LMAO, What register is this????");
        }
    }
}

#endif //LLVM_TL45BASEINFO_H
