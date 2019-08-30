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
    FunctionPass *createLC2200ISelDag(LC2200TargetMachine &TM, CodeGenOpt::Level OptLevel);
} // end namespace llvm;

#endif

