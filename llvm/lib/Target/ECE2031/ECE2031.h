//
// Created by codetector on 8/29/19.
//

#ifndef TARGET_ECE2031_H
#define TARGET_ECE2031_H
#include "MCTargetDesc/ECE2031MCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class TargetMachine;
    class ECE2031TargetMachine;
    class MCInst;

namespace ECE2031ABI {

enum ABI {
  ABI_ILP32,
  ABI_ILP32F,
  ABI_ILP32D,
  ABI_ILP32E,
  ABI_LP64,
  ABI_LP64F,
  ABI_LP64D,
  ABI_Unknown
};

}

    FunctionPass *createECE2031ISelDag(ECE2031TargetMachine &TM, CodeGenOpt::Level OptLevel);
} // end namespace llvm;

#endif

