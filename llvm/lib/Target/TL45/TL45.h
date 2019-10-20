//
// Created by codetector on 8/29/19.
//

#ifndef TARGET_TL45_H
#define TARGET_TL45_H
#include "MCTargetDesc/TL45MCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class TargetMachine;
    class TL45TargetMachine;
    class MCInst;

namespace TL45ABI {

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

    FunctionPass *createTL45ISelDag(TL45TargetMachine &TM, CodeGenOpt::Level OptLevel);
} // end namespace llvm;

#endif

