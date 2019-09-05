//
// Created by codetector on 8/29/19.
//

#ifndef TARGET_LC2200_H
#define TARGET_LC2200_H
#include "MCTargetDesc/LC2200MCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class TargetMachine;
    class LC2200TargetMachine;
    class MCInst;

namespace LC2200ABI {

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

    FunctionPass *createLC2200ISelDag(LC2200TargetMachine &TM, CodeGenOpt::Level OptLevel);
} // end namespace llvm;

#endif

