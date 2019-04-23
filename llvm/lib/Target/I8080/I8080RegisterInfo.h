//
// Created by codetector on 4/22/19.
//

#ifndef TARGET_I8080REGISTERINFO_H
#define TARGET_I8080REGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER

#include "I8080GenRegisterInfo.inc"

namespace llvm {
    class TargetInstrInfo;

    class I8080RegisterInfo final : public I8080GenRegisterInfo {
    };

}


#endif //TARGET_I8080REGISTERINFO_H
