//
// Created by Codetector on 2019-07-11.
//

#ifndef LLVM_LC3MCTARGETDESC_H
#define LLVM_LC3MCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {
    class Target;

    extern Target TheLC3Target;
}

#define GET_REGINFO_ENUM
#include "LC3GenRegisterInfo.inc"


#define GET_INSTRINFO_ENUM
#include "LC3GenInstrInfo.inc"

#endif //LLVM_LC3MCTARGETDESC_H
