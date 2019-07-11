//
// Created by codetector on 5/5/19.
//

#include "LC3TargetMachine.h"
#include "LC3.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

static std::string computeDataLayout(const Triple &TT, StringRef CPU,
                                     const TargetOptions &Options) {
    return "e-m:e-p:16:16:16-i1:16:16-i8:8:8-i16:16:16-i32:32:32-f64:64:64-a:0:16-n16";
}

LC3TargetMachine::LC3TargetMachine(const llvm::Target &T, const llvm::Triple &TT, llvm::StringRef CPU,
                                   llvm::StringRef FS, const llvm::TargetOptions &Options,
                                   Optional<llvm::Reloc::Model> RM,
                                   Optional<llvm::CodeModel::Model> CM, llvm::CodeGenOpt::Level OL, bool JIT) :
                                   TargetMachine(T, computeDataLayout(TT, CPU, Options), TT, CPU, FS, Options
) {

}

extern "C" void LLVMInitializeLC3Target() {
    RegisterTargetMachine<LC3TargetMachine> X(TheLC3Target);
}

